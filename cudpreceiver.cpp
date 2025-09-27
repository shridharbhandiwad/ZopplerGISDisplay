#include "cudpreceiver.h"

/**
 * @brief CUdpReceiver constructor
 *        Initializes and moves the receiver to a separate thread.
 */
CUdpReceiver::CUdpReceiver(QObject *parent) : QObject(parent)
{
    // Move this object to the worker thread
    this->moveToThread(&m_workerThread);

    // Connect thread finish signal to delete this object safely
    connect(&m_workerThread, &QThread::finished, this, &QObject::deleteLater);

    // Start the thread
    m_workerThread.start();
}


/**
 * @brief CUdpReceiver destructor
 *        Cleans up the thread and socket
 */
CUdpReceiver::~CUdpReceiver()
{
    stopListening();
    m_workerThread.quit();
    m_workerThread.wait();
}

/**
 * @brief Starts listening on the given UDP port
 * @param nPort The port to bind the UDP socket to
 */
void CUdpReceiver::startListening(quint16 nPort)
{
    m_nListeningPort = nPort;

    // Run this in the receiver thread
    QMetaObject::invokeMethod(this, [this]() {
        m_pUdpSocket = new QUdpSocket();

        // Bind to the given port
        if (!m_pUdpSocket->bind(QHostAddress::AnyIPv4, m_nListeningPort)) {
            qCritical() << "[CUdpReceiver] Failed to bind to port"
                        << m_nListeningPort << ":" << m_pUdpSocket->errorString();
            delete m_pUdpSocket;
            m_pUdpSocket = nullptr;
            return;
        }

        // Connect readyRead to our processing slot
        connect(m_pUdpSocket, &QUdpSocket::readyRead,
                this, &CUdpReceiver::_processPendingDatagrams);

        qDebug() << "[CUdpReceiver] Listening on port" << m_nListeningPort;
    });
}

/**
 * @brief Stops listening and cleans up the socket
 */
void CUdpReceiver::stopListening()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_pUdpSocket) {
            m_pUdpSocket->close();
            m_pUdpSocket->deleteLater();
            m_pUdpSocket = nullptr;
            qDebug() << "[CUdpReceiver] Stopped listening on port" << m_nListeningPort;
        }
    });
}

/**
 * @brief Handles incoming datagrams and emits track data signals
 */
void CUdpReceiver::_processPendingDatagrams()
{
    while (m_pUdpSocket && m_pUdpSocket->hasPendingDatagrams()) {
        QByteArray baDatagram;
        baDatagram.resize(static_cast<int>(m_pUdpSocket->pendingDatagramSize()));

        QHostAddress sender;
        quint16 nSenderPort;

        if (m_pUdpSocket->readDatagram(baDatagram.data(), baDatagram.size(),
                                       &sender, &nSenderPort) == -1) {
            qWarning() << "[CUdpReceiver] Failed to read datagram:"
                       << m_pUdpSocket->errorString();
            continue;
        }



        // Check size of received datagram
        if (baDatagram.size() == sizeof(stTrackRecvInfo)) {
            stTrackRecvInfo stTrack;
            memcpy(&stTrack, baDatagram.constData(), sizeof(stTrackRecvInfo));

            // Emit signal with parsed track
            emit signalUpdateTrackData(stTrack);
        } else {
            qWarning() << "[CUdpReceiver] Invalid datagram size:" << baDatagram.size()
                       << ", expected:" << sizeof(stTrackRecvInfo);
        }
    }
}
