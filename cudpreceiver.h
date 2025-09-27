#ifndef CUDPRECEIVER_H
#define CUDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include "globalstructs.h"

class CUdpReceiver : public QObject
{
    Q_OBJECT
public:
    explicit CUdpReceiver(QObject *parent = nullptr);

    /**
        * @brief Destructor
        */
       ~CUdpReceiver();

       /**
        * @brief Start listening to a specified UDP port
        * @param nPort UDP port to listen on
        */
       void startListening(quint16 nPort);

       /**
        * @brief Stop listening and clean up resources
        */
       void stopListening();

   signals:
       /**
        * @brief Signal emitted when a valid TrackToDisplay struct is received
        * @param stTrack The received track data
        */
       void signalUpdateTrackData(stTrackRecvInfo stTrack);

   private slots:
       /**
        * @brief Process incoming datagrams from the UDP socket
        */
       void _processPendingDatagrams();

   private:
       QUdpSocket *m_pUdpSocket = nullptr;    //!< UDP socket for receiving data
       QThread m_workerThread;                //!< Thread in which the receiver runs
       quint16 m_nListeningPort = 0;          //!< Port number to listen on
};

#endif // CUDPRECEIVER_H
