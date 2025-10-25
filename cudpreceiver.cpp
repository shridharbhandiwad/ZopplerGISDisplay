#include "cudpreceiver.h"
#include <QDebug>

/**
 * @brief CUdpReceiver constructor
 *        Initializes the UDP receiver (runs in the same thread as parent)
 */
CUdpReceiver::CUdpReceiver(QObject *parent) : QObject(parent)
{
    qDebug() << "[CUdpReceiver] Constructor called";
}


/**
 * @brief CUdpReceiver destructor
 *        Cleans up the socket
 */
CUdpReceiver::~CUdpReceiver()
{
    stopListening();
}

/**
 * @brief Starts listening on the given UDP port
 * @param nPort The port to bind the UDP socket to
 */
void CUdpReceiver::startListening(quint16 nPort)
{
    m_nListeningPort = nPort;

    // Clean up any existing socket
    if (m_pUdpSocket) {
        stopListening();
    }

    // Create UDP socket
    m_pUdpSocket = new QUdpSocket(this);

    // Bind to the given port
    if (!m_pUdpSocket->bind(QHostAddress::AnyIPv4, m_nListeningPort)) {
        qCritical() << "[CUdpReceiver] Failed to bind to port"
                    << m_nListeningPort << ":" << m_pUdpSocket->errorString();
        delete m_pUdpSocket;
        m_pUdpSocket = nullptr;
        return;
    }

    // Connect readyRead to our processing slot - direct connection for immediate processing
    connect(m_pUdpSocket, &QUdpSocket::readyRead,
            this, &CUdpReceiver::_processPendingDatagrams, Qt::DirectConnection);

    qDebug() << "[CUdpReceiver] Successfully listening on port" << m_nListeningPort;
}

/**
 * @brief Stops listening and cleans up the socket
 */
void CUdpReceiver::stopListening()
{
    if (m_pUdpSocket) {
        m_pUdpSocket->close();
        m_pUdpSocket->deleteLater();
        m_pUdpSocket = nullptr;
        qDebug() << "[CUdpReceiver] Stopped listening on port" << m_nListeningPort;
    }
}

/**
 * @brief Handles incoming datagrams and emits track data signals immediately
 */
void CUdpReceiver::_processPendingDatagrams()
{
    qDebug() << "[CUdpReceiver] _processPendingDatagrams() called - data available!";
    
    while (m_pUdpSocket && m_pUdpSocket->hasPendingDatagrams()) {
        QByteArray baDatagram;
        qint64 datagramSize = m_pUdpSocket->pendingDatagramSize();
        baDatagram.resize(static_cast<int>(datagramSize));

        QHostAddress sender;
        quint16 nSenderPort;

        qint64 bytesRead = m_pUdpSocket->readDatagram(baDatagram.data(), baDatagram.size(),
                                                      &sender, &nSenderPort);
        
        if (bytesRead == -1) {
            qWarning() << "[CUdpReceiver] Failed to read datagram:"
                       << m_pUdpSocket->errorString();
            continue;
        }

        qDebug() << "[CUdpReceiver] Datagram received from" << sender.toString() 
                 << "port" << nSenderPort << "size:" << bytesRead << "bytes";

        // Check size of received datagram
        if (baDatagram.size() == sizeof(stTrackRecvInfo)) {
            stTrackRecvInfo stTrack;
            memcpy(&stTrack, baDatagram.constData(), sizeof(stTrackRecvInfo));

            qDebug() << "[CUdpReceiver] Parsed track - MsgID:" << stTrack.usMsgId
                     << "TrackID:" << stTrack.nTrkId 
                     << "Pos(X,Y,Z):" << stTrack.x << stTrack.y << stTrack.z
                     << "Heading:" << stTrack.heading << "Velocity:" << stTrack.velocity;
            
            // Emit signal with parsed track - IMMEDIATE emission
            qDebug() << "[CUdpReceiver] About to emit signal for track ID:" << stTrack.nTrkId;
            emit signalUpdateTrackData(stTrack);
            qDebug() << "[CUdpReceiver] Signal emitted successfully for track ID:" << stTrack.nTrkId;
        } else {
            qWarning() << "[CUdpReceiver] Invalid datagram size:" << baDatagram.size()
                       << "bytes, expected:" << sizeof(stTrackRecvInfo) << "bytes";
        }
    }
    
    qDebug() << "[CUdpReceiver] _processPendingDatagrams() completed";
}
