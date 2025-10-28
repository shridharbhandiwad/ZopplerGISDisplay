#ifndef CGPSUDPRECEIVER_H
#define CGPSUDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include "globalstructs.h"

class CGpsUdpReceiver : public QObject
{
    Q_OBJECT
public:
    explicit CGpsUdpReceiver(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~CGpsUdpReceiver();

    /**
     * @brief Start listening to a specified UDP port for GPS data
     * @param nPort UDP port to listen on
     */
    void startListening(quint16 nPort);

    /**
     * @brief Stop listening and clean up resources
     */
    void stopListening();

signals:
    /**
     * @brief Signal emitted when valid GPS data is received
     * @param gpsData The received GPS data
     */
    void signalUpdateGpsData(stGpsRecvInfo gpsData);

private slots:
    /**
     * @brief Process incoming datagrams from the UDP socket
     */
    void _processPendingDatagrams();

private:
    QUdpSocket *m_pUdpSocket = nullptr;    //!< UDP socket for receiving data
    QThread m_workerThread;                //!< Thread in which the receiver runs
    quint16 m_nListeningPort = 0;          //!< Port number to listen on

    /**
     * @brief Parse JSON GPS data from UDP packet
     * @param jsonData JSON string containing GPS data
     * @param gpsData Output GPS data structure
     * @return true if parsing successful, false otherwise
     */
    bool parseJsonGpsData(const QByteArray &jsonData, stGpsRecvInfo &gpsData);

    /**
     * @brief Parse binary GPS data from UDP packet
     * @param binaryData Binary data containing GPS structure
     * @param gpsData Output GPS data structure
     * @return true if parsing successful, false otherwise
     */
    bool parseBinaryGpsData(const QByteArray &binaryData, stGpsRecvInfo &gpsData);
};

#endif // CGPSUDPRECEIVER_H