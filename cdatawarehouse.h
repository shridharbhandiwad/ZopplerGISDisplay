#ifndef CDATAWAREHOUSE_H
#define CDATAWAREHOUSE_H

#include <QObject>
#include "globalstructs.h"
#include <QMutex>
#include "cudpreceiver.h"
#include "CoordinateConverter.h"
#include <QPointF>
#include <QTimer>

class CDataWarehouse : public QObject
{
    Q_OBJECT

public :
    /**
     * @brief Gets the singleton instance of CDataWarehouse
     * @return Pointer to the singleton instance
     */
    static CDataWarehouse* getInstance();

    QList<stTrackDisplayInfo> getTrackList();

    const QPointF getRadarPos();
private slots:
    void slotUpdateTrackData(stTrackRecvInfo trackRecvInfo);
    void slotClearTracksOnTimeOut();
private:
    /**
     * @brief Private constructor for singleton pattern
     * @param pParent Optional QObject parent pointer
     */
    explicit CDataWarehouse(QObject *pParent = nullptr);

    QTimer _m_timeTrackTimeout;

    static CDataWarehouse*_m_pInstance;  //!< Singleton instance pointer
    static QMutex _m_mutex;                //!< Mutex for thread-safe singleton initialization


    QHash<int,stTrackDisplayInfo> _m_listTrackInfo;

    CUdpReceiver _m_UdpRecvr;

    CoordinateConverter _m_CoordConv;

    QPointF _m_RadarPos;

};

#endif // CDATAWAREHOUSE_H
