# GPS Radar Display - Qt5 QGIS Application

A Qt5 GUI application that receives GPS coordinates (latitude, longitude, altitude) via UDP and displays the positions on a global map using QGIS plugins.

## Features

- **UDP GPS Data Reception**: Receives GPS data via UDP on port 2026
- **Dual Data Format Support**: Accepts both JSON and binary GPS data formats
- **Real-time Map Display**: Shows GPS positions on a QGIS-powered map
- **Track Management**: Displays multiple tracks with different identities
- **Coordinate Conversion**: Converts between GPS coordinates and local coordinate systems
- **Modern Qt5 Interface**: Clean, responsive GUI with dockable panels

## Prerequisites

### System Requirements
- Linux (tested on Ubuntu/Debian)
- Qt5.15.3 or later
- QGIS 3.x development libraries
- CMake 3.16+ or qmake

### Dependencies Installation (Ubuntu/Debian)

```bash
# Install Qt5 development packages
sudo apt-get update
sudo apt-get install qt5-default qtbase5-dev qttools5-dev-tools
sudo apt-get install qtquickcontrols2-5-dev qml-module-qtquick-controls2
sudo apt-get install libqt5charts5-dev libqt5quickwidgets5

# Install QGIS development packages
sudo apt-get install qgis-dev libqgis-dev
sudo apt-get install libgeos-c1v5 libproj-dev libspatialindex-dev

# Install CMake (if using CMake build)
sudo apt-get install cmake
```

## Building the Application

### Option 1: Using CMake (Recommended)

```bash
# Create build directory
mkdir build && cd build

# Configure the project
cmake ..

# Build the application
make -j$(nproc)

# The executable will be in build/bin/GpsRadarDisplay
```

### Option 2: Using qmake

```bash
# Generate Makefile
qmake RadarDisplay.pro

# Build the application
make -j$(nproc)

# The executable will be in ../bin/RadarDisplay
```

## GPS Data Formats

The application accepts GPS data via UDP on **port 2026** in two formats:

### JSON Format (Recommended)
```json
{
  "msgId": 1,
  "trackId": 123,
  "lat": 13.2716,
  "lon": 77.2946,
  "alt": 915.0,
  "heading": 45.5,
  "velocity": 25.3,
  "identity": 2
}
```

### Binary Format
A packed C structure (`stGpsRecvInfo`) containing:
- `unsigned short usMsgId`: Message ID
- `int nTrkId`: Track ID
- `double lat`: Latitude (degrees)
- `double lon`: Longitude (degrees)  
- `double alt`: Altitude (meters)
- `float heading`: Heading (degrees)
- `float velocity`: Velocity (m/s)
- `int nTrackIden`: Track identity (0=Default, 1=Unknown, 2=Friend, 3=Hostile)

## Running the Application

1. **Start the Application**:
   ```bash
   # From build directory (CMake)
   ./bin/GpsRadarDisplay
   
   # Or from project root (qmake)
   ../bin/RadarDisplay
   ```

2. **Send Test GPS Data**:
   ```bash
   # Make the test sender executable
   chmod +x gps_test_sender.py
   
   # Send static test positions
   python3 gps_test_sender.py --mode static
   
   # Simulate a moving track
   python3 gps_test_sender.py --mode moving --track-id 100 --velocity 60 --heading 90 --duration 120
   ```

## Application Interface

### Main Components

1. **Map Canvas**: Central QGIS map display showing GPS positions
2. **Controls Panel**: Map navigation and control buttons (toggle with 'F' key)
3. **Tracks Table**: Real-time track information display (toggle with 'T' key)
4. **Dashboard**: Quick UI panel with additional controls

### Keyboard Shortcuts

- **F**: Toggle Controls panel visibility
- **T**: Toggle Tracks table visibility  
- **H**: Return map to home position

### Map Controls

- **Mouse Wheel**: Zoom in/out
- **Left Click + Drag**: Pan the map
- **Map Home**: Reset map to default view
- **Open Maps**: Load raster map files (GeoTIFF, etc.)

## GPS Test Sender Usage

The included `gps_test_sender.py` script helps test the application:

### Static Test Data
```bash
python3 gps_test_sender.py --mode static --host 127.0.0.1 --port 2026
```

### Moving Simulation
```bash
# Simulate aircraft moving northeast at 100 km/h for 5 minutes
python3 gps_test_sender.py --mode moving \
  --track-id 200 \
  --start-lat 13.2716 --start-lon 77.2946 --start-alt 1000 \
  --heading 45 --velocity 100 \
  --duration 300 --interval 2
```

### Parameters
- `--host`: Target IP address (default: 127.0.0.1)
- `--port`: Target UDP port (default: 2026)
- `--track-id`: Unique track identifier
- `--start-lat/lon/alt`: Starting GPS position
- `--heading`: Movement direction in degrees (0°=North, 90°=East)
- `--velocity`: Speed in km/h
- `--duration`: Simulation time in seconds
- `--interval`: Update frequency in seconds

## Configuration

### Default Settings
- **GPS UDP Port**: 2026
- **Legacy UDP Port**: 2025 (for existing Cartesian data)
- **Reference Position**: Bangalore, India (13.2716°N, 77.2946°E)
- **Track Timeout**: 10 seconds (tracks disappear if no updates)

### Customization
Edit `cdatawarehouse.cpp` to modify:
- Reference radar position (`_m_RadarPos`)
- UDP listening ports
- Track timeout duration
- Default track parameters

## Troubleshooting

### Common Issues

1. **QGIS Libraries Not Found**:
   ```bash
   sudo apt-get install qgis-dev libqgis-dev
   # Or check paths in CMakeLists.txt
   ```

2. **Qt5 Components Missing**:
   ```bash
   sudo apt-get install qt5-default qtquickcontrols2-5-dev
   ```

3. **UDP Port Already in Use**:
   - Check if another instance is running
   - Modify port numbers in `cdatawarehouse.cpp`

4. **Map Not Loading**:
   - Ensure QGIS is properly installed
   - Check console output for QGIS initialization errors

### Debug Output
The application provides detailed console output for:
- UDP data reception
- GPS coordinate parsing
- Track updates
- Map operations

## Architecture Overview

### Key Components

- **`CGpsUdpReceiver`**: Handles UDP GPS data reception and parsing
- **`CDataWarehouse`**: Central data management and coordinate conversion
- **`CMapCanvas`**: QGIS-based map display and interaction
- **`CMapMainWindow`**: Main application window and UI management
- **Track Layers**: QGIS vector layers for displaying GPS positions

### Data Flow

1. GPS data received via UDP (port 2026)
2. Parsed as JSON or binary format
3. Converted to display coordinates
4. Updated in track database
5. Rendered on QGIS map canvas
6. Displayed in tracks table

## License

This project uses Qt5 (LGPL) and QGIS libraries. See individual component licenses for details.

## Support

For issues or questions:
1. Check the console output for error messages
2. Verify QGIS and Qt5 installation
3. Test with the provided GPS sender script
4. Ensure UDP port 2026 is available