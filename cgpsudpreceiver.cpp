#include "cgpsudpreceiver.h"
#include <QDebug>
#include <QJsonParseError>

/**
 * @brief CGpsUdpReceiver constructor
 *        Initializes and moves the receiver to a separate thread.
 */
CGpsUdpReceiver::CGpsUdpReceiver(QObject *parent) : QObject(parent)
{
    // Move this object to the worker thread
    this->moveToThread(&m_workerThread);

    // Connect thread finish signal to delete this object safely
    connect(&m_workerThread, &QThread::finished, this, &QObject::deleteLater);

    // Start the thread
    m_workerThread.start();
}

/**
 * @brief CGpsUdpReceiver destructor
 *        Cleans up the thread and socket
 */
CGpsUdpReceiver::~CGpsUdpReceiver()
{
    stopListening();
    m_workerThread.quit();
    m_workerThread.wait();
}

/**
 * @brief Starts listening on the given UDP port for GPS data
 * @param nPort The port to bind the UDP socket to
 */
void CGpsUdpReceiver::startListening(quint16 nPort)
{
    m_nListeningPort = nPort;

    // Run this in the receiver thread
    QMetaObject::invokeMethod(this, [this]() {
        m_pUdpSocket = new QUdpSocket();

        // Bind to the given port
        if (!m_pUdpSocket->bind(QHostAddress::AnyIPv4, m_nListeningPort)) {
            qCritical() << "[CGpsUdpReceiver] Failed to bind to port"
                        << m_nListeningPort << ":" << m_pUdpSocket->errorString();
            delete m_pUdpSocket;
            m_pUdpSocket = nullptr;
            return;
        }

        // Connect readyRead to our processing slot
        connect(m_pUdpSocket, &QUdpSocket::readyRead,
                this, &CGpsUdpReceiver::_processPendingDatagrams);

        qDebug() << "[CGpsUdpReceiver] Listening for GPS data on port" << m_nListeningPort;
    });
}

/**
 * @brief Stops listening and cleans up the socket
 */
void CGpsUdpReceiver::stopListening()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_pUdpSocket) {
            m_pUdpSocket->close();
            m_pUdpSocket->deleteLater();
            m_pUdpSocket = nullptr;
            qDebug() << "[CGpsUdpReceiver] Stopped listening on port" << m_nListeningPort;
        }
    });
}

/**
 * @brief Handles incoming datagrams and emits GPS data signals
 */
void CGpsUdpReceiver::_processPendingDatagrams()
{
    while (m_pUdpSocket && m_pUdpSocket->hasPendingDatagrams()) {
        QByteArray baDatagram;
        baDatagram.resize(static_cast<int>(m_pUdpSocket->pendingDatagramSize()));

        QHostAddress sender;
        quint16 nSenderPort;

        if (m_pUdpSocket->readDatagram(baDatagram.data(), baDatagram.size(),
                                       &sender, &nSenderPort) == -1) {
            qWarning() << "[CGpsUdpReceiver] Failed to read datagram:"
                       << m_pUdpSocket->errorString();
            continue;
        }

        stGpsRecvInfo gpsData;

        // Try to parse as JSON first (more flexible format)
        if (parseJsonGpsData(baDatagram, gpsData)) {
            qDebug() << "[CGpsUdpReceiver] Received GPS JSON data - Track ID:" << gpsData.nTrkId
                     << "Lat:" << gpsData.lat << "Lon:" << gpsData.lon << "Alt:" << gpsData.alt;
            emit signalUpdateGpsData(gpsData);
        }
        // Try to parse as binary structure
        else if (parseBinaryGpsData(baDatagram, gpsData)) {
            qDebug() << "[CGpsUdpReceiver] Received GPS binary data - Track ID:" << gpsData.nTrkId
                     << "Lat:" << gpsData.lat << "Lon:" << gpsData.lon << "Alt:" << gpsData.alt;
            emit signalUpdateGpsData(gpsData);
        }
        else {
            qWarning() << "[CGpsUdpReceiver] Failed to parse GPS data from" << sender.toString()
                       << "Size:" << baDatagram.size() << "Data:" << baDatagram.left(100);
        }
    }
}

/**
 * @brief Parse JSON GPS data from UDP packet
 * Expected JSON format:
 * {
 *   "msgId": 1,
 *   "trackId": 123,
 *   "lat": 13.2716,
 *   "lon": 77.2946,
 *   "alt": 915.0,
 *   "heading": 45.5,
 *   "velocity": 25.3,
 *   "identity": 2
 * }
 */
bool CGpsUdpReceiver::parseJsonGpsData(const QByteArray &jsonData, stGpsRecvInfo &gpsData)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    if (!doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();

    // Check for required fields
    if (!obj.contains("lat") || !obj.contains("lon") || !obj.contains("alt")) {
        return false;
    }

    // Parse GPS data
    gpsData.usMsgId = static_cast<unsigned short>(obj.value("msgId").toInt(1));
    gpsData.nTrkId = obj.value("trackId").toInt(1);
    gpsData.lat = obj.value("lat").toDouble();
    gpsData.lon = obj.value("lon").toDouble();
    gpsData.alt = obj.value("alt").toDouble();
    gpsData.heading = static_cast<float>(obj.value("heading").toDouble(0.0));
    gpsData.velocity = static_cast<float>(obj.value("velocity").toDouble(0.0));
    gpsData.nTrackIden = obj.value("identity").toInt(TRACK_IDENTITY_DEFAULT);

    // Basic validation
    if (gpsData.lat < -90.0 || gpsData.lat > 90.0 ||
        gpsData.lon < -180.0 || gpsData.lon > 180.0) {
        qWarning() << "[CGpsUdpReceiver] Invalid GPS coordinates - Lat:" << gpsData.lat << "Lon:" << gpsData.lon;
        return false;
    }

    return true;
}

/**
 * @brief Parse binary GPS data from UDP packet
 */
bool CGpsUdpReceiver::parseBinaryGpsData(const QByteArray &binaryData, stGpsRecvInfo &gpsData)
{
    // Check size of received datagram
    if (binaryData.size() != sizeof(stGpsRecvInfo)) {
        return false;
    }

    memcpy(&gpsData, binaryData.constData(), sizeof(stGpsRecvInfo));

    // Basic validation
    if (gpsData.lat < -90.0 || gpsData.lat > 90.0 ||
        gpsData.lon < -180.0 || gpsData.lon > 180.0) {
        qWarning() << "[CGpsUdpReceiver] Invalid GPS coordinates - Lat:" << gpsData.lat << "Lon:" << gpsData.lon;
        return false;
    }

    return true;
}