#ifndef GLOBALSTRUCTS_H
#define GLOBALSTRUCTS_H


enum eTrackIdentity {
    TRACK_IDENTITY_DEFAULT = 0,
    TRACK_IDENTITY_UNKNOWN = 1,
    TRACK_IDENTITY_FRIEND = 2,
    TRACK_IDENTITY_HOSTILE = 3,
};

#pragma pack(1)
// Replace with your actual structure definition
struct stTrackRecvInfo {
    unsigned short usMsgId;    //!< Msg ID
    int nTrkId;                //!< Track ID
    float x;                   //!< X-coordinate
    float y;                   //!< Y-coordinate
    float z;                   //!< Z-coordinate
    float heading;             //!< Heading
    float velocity;            //!< Velocity
    int nTrackIden;
};

// GPS data structure for direct lat/lon/alt reception
struct stGpsRecvInfo {
    unsigned short usMsgId;    //!< Message ID
    int nTrkId;                //!< Track ID
    double lat;                //!< Latitude (degrees)
    double lon;                //!< Longitude (degrees)
    double alt;                //!< Altitude (meters)
    float heading;             //!< Heading (degrees)
    float velocity;            //!< Velocity (m/s)
    int nTrackIden;            //!< Track identity
};

struct stTrackDisplayInfo {
    int nTrkId;                 //!< Track ID
    float x;                    //!< X-coordinate
    float y;                    //!< Y-coordinate
    float z;                    //!< Z-coordinate
    double lat;                 //!< Latitude
    double lon;                 //!< Longitude
    double alt;                 //!< Altitude
    double range;               //!< Range
    double azimuth;             //!< Azimuth
    double elevation;           //!< Elevation
    double heading;             //!< Heading
    double velocity;            //!< Velocity
    double snr;                 //!< SNR
    int nTrackIden;
    long long nTrackTime;
};

#pragma pack()

#endif // GLOBALSTRUCTS_H
