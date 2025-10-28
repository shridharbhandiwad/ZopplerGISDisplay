#include "cdatawarehouse.h"
#include <QMutexLocker>
#include "cudpreceiver.h"
#include <QDateTime>

// Initialize static member variables
CDataWarehouse* CDataWarehouse::_m_pInstance = nullptr;
QMutex CDataWarehouse::_m_mutex;

CDataWarehouse* CDataWarehouse::getInstance()
{
    // Thread-safe singleton instantiation using mutex lock
    if (_m_pInstance == nullptr) {
        QMutexLocker locker(&_m_mutex);
        if (_m_pInstance == nullptr) {
            _m_pInstance = new CDataWarehouse();
        }
    }
    return _m_pInstance;
}

CDataWarehouse::CDataWarehouse(QObject *parent) : QObject(parent)
{
    _m_RadarPos = QPointF(77.2946, 13.2716);

    stTrackRecvInfo info1;
    info1.nTrkId = 1;
    info1.x = 7000;
    info1.y = 0;
    info1.z = 0;
    info1.velocity = 20;
    info1.heading = 100;
    info1.nTrackIden = TRACK_IDENTITY_FRIEND;

    stTrackRecvInfo info2;
    info2.nTrkId = 2;
    info2.x = -4000;
    info2.y = 0;
    info2.z = 0;
    info2.velocity = 20;
    info2.heading = 200;
    info2.nTrackIden = TRACK_IDENTITY_HOSTILE;

    stTrackRecvInfo info3;
    info3.nTrkId = 3;
    info3.x = -2000;
    info3.y = 0;
    info3.z = 0;
    info3.velocity = 35;
    info3.heading = 200;
    info3.nTrackIden = TRACK_IDENTITY_UNKNOWN;


    slotUpdateTrackData(info1);
    slotUpdateTrackData(info2);
    slotUpdateTrackData(info3);

    connect(&_m_timeTrackTimeout,SIGNAL(timeout()),this,SLOT(slotClearTracksOnTimeOut()));
    _m_timeTrackTimeout.start(1000);

    _m_UdpRecvr.startListening(2025);
    connect(&_m_UdpRecvr,SIGNAL(signalUpdateTrackData(stTrackRecvInfo)),this,SLOT(slotUpdateTrackData(stTrackRecvInfo)));

    // Start GPS UDP receiver on port 2026
    _m_GpsUdpRecvr.startListening(2026);
    connect(&_m_GpsUdpRecvr,SIGNAL(signalUpdateGpsData(stGpsRecvInfo)),this,SLOT(slotUpdateGpsData(stGpsRecvInfo)));
}

QList<stTrackDisplayInfo> CDataWarehouse::getTrackList() {
    return _m_listTrackInfo.values();
}

void CDataWarehouse::slotClearTracksOnTimeOut() {

    QList<int> trackIds = _m_listTrackInfo.keys();
    for ( int trackId : trackIds  ) {
        stTrackDisplayInfo trackInfo = _m_listTrackInfo.value(trackId);
        if ( (trackInfo.nTrackTime + 10) < QDateTime::currentDateTime().toSecsSinceEpoch() ) {
            _m_listTrackInfo.remove(trackInfo.nTrkId);
        }
    }
}

void CDataWarehouse::slotUpdateTrackData(stTrackRecvInfo trackRecvInfo) {

    stTrackDisplayInfo info;
    info.nTrkId = trackRecvInfo.nTrkId;
    info.heading = trackRecvInfo.heading;
    info.nTrackIden = trackRecvInfo.nTrackIden;
    info.nTrackTime = QDateTime::currentDateTime().toSecsSinceEpoch();

    _m_CoordConv.env2geodetic(trackRecvInfo.x,trackRecvInfo.y,trackRecvInfo.z,
                              _m_RadarPos.y(),_m_RadarPos.x(),0,&info.lat,&info.lon,&info.alt,0);

    _m_CoordConv.env2polar(&info.range,&info.azimuth,&info.elevation,
                           trackRecvInfo.x,trackRecvInfo.y,trackRecvInfo.z);

    _m_listTrackInfo.insert(info.nTrkId,info);
}

void CDataWarehouse::slotUpdateGpsData(stGpsRecvInfo gpsRecvInfo) {
    stTrackDisplayInfo info;
    info.nTrkId = gpsRecvInfo.nTrkId;
    info.heading = gpsRecvInfo.heading;
    info.velocity = gpsRecvInfo.velocity;
    info.nTrackIden = gpsRecvInfo.nTrackIden;
    info.nTrackTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    
    // GPS data is already in lat/lon/alt format
    info.lat = gpsRecvInfo.lat;
    info.lon = gpsRecvInfo.lon;
    info.alt = gpsRecvInfo.alt;
    
    // Convert GPS coordinates to local Cartesian coordinates for range/azimuth calculation
    _m_CoordConv.geodetic2env(gpsRecvInfo.lat, gpsRecvInfo.lon, gpsRecvInfo.alt,
                              _m_RadarPos.y(), _m_RadarPos.x(), 0, 
                              &info.x, &info.y, &info.z, 0);
    
    // Calculate range, azimuth, elevation from Cartesian coordinates
    _m_CoordConv.env2polar(&info.range, &info.azimuth, &info.elevation,
                           info.x, info.y, info.z);
    
    // Set SNR to a default value (not provided in GPS data)
    info.snr = 20.0; // Default SNR value
    
    _m_listTrackInfo.insert(info.nTrkId, info);
    
    qDebug() << "[CDataWarehouse] Updated GPS track" << info.nTrkId 
             << "at" << info.lat << "," << info.lon << "alt:" << info.alt
             << "range:" << info.range << "azimuth:" << info.azimuth;
}

const QPointF CDataWarehouse::getRadarPos() {
    return _m_RadarPos;
}
