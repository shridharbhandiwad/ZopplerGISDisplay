# GPS Radar Display - Qt5 QGIS Application

## Project Overview

This project provides a complete Qt5 GUI application that receives GPS coordinates (latitude, longitude, altitude) via UDP and displays the positions on a global map using QGIS plugins. The application is designed for real-time tracking and visualization of multiple GPS-enabled objects.

## ✅ Completed Components

### 1. Core Application Structure
- **Qt5 Application Framework**: Complete Qt5 application with modern GUI
- **QGIS Integration**: Full QGIS map canvas integration for professional mapping
- **Multi-threaded Architecture**: Separate threads for UDP reception and GUI updates
- **Dockable Interface**: Professional interface with dockable panels for controls and data

### 2. GPS UDP Reception System
- **Dual Format Support**: Handles both JSON and binary GPS data formats
- **Thread-safe Reception**: Multi-threaded UDP receiver with proper synchronization
- **Data Validation**: Comprehensive GPS coordinate validation and error handling
- **Flexible Port Configuration**: Configurable UDP ports (default: 2026 for GPS data)

### 3. Data Management
- **Real-time Processing**: Live GPS data processing and coordinate conversion
- **Track Management**: Automatic track creation, updating, and timeout handling
- **Coordinate Conversion**: Conversion between GPS coordinates and local coordinate systems
- **Identity Management**: Support for different track identities (Friend, Hostile, Unknown)

### 4. Map Display Features
- **QGIS-powered Mapping**: Professional-grade mapping with QGIS backend
- **Real-time Updates**: Live position updates on the map
- **Interactive Controls**: Pan, zoom, and navigation controls
- **Raster Map Support**: Load and display various map formats (GeoTIFF, etc.)
- **Vector Layers**: Custom vector layers for track display

### 5. User Interface
- **Main Map Canvas**: Central QGIS map display
- **Controls Panel**: Map navigation and control buttons
- **Tracks Table**: Real-time track information display
- **Dashboard**: Quick UI panel with additional controls
- **Keyboard Shortcuts**: Efficient keyboard navigation

### 6. Build System
- **CMake Support**: Modern CMake build system with dependency detection
- **qmake Support**: Traditional Qt qmake build system
- **Automated Build Script**: Comprehensive build script with dependency checking
- **Cross-platform Compatibility**: Linux-focused with portable design

### 7. Testing and Validation
- **GPS Test Sender**: Python script for sending test GPS data
- **Static Test Mode**: Send predefined GPS positions for testing
- **Moving Simulation**: Simulate moving tracks with realistic GPS data
- **Console Demo**: Simplified console application for testing UDP reception

## 📁 Project Files

### Core Application Files
- `main.cpp` - Application entry point and QGIS initialization
- `cmapmainwindow.h/cpp` - Main window implementation
- `cmapmainwindow.ui` - UI layout definition
- `globalstructs.h` - Data structures and enumerations
- `globalmacros.h` - Application constants and macros

### GPS Reception System
- `cgpsudpreceiver.h/cpp` - GPS UDP receiver class (NEW)
- `cudpreceiver.h/cpp` - Legacy UDP receiver for Cartesian data
- `cdatawarehouse.h/cpp` - Central data management and coordinate conversion

### Map Display Components
- `MapDisplay/cmapcanvas.h/cpp` - QGIS map canvas implementation
- `MapDisplay/cppilayer.h/cpp` - PPI radar layer
- `MapDisplay/ctracklayer.h/cpp` - Track display layer
- `MapDisplay/csearchbeamlayer.h/cpp` - Search beam visualization
- `MapDisplay/customgradiantfillsymbollayer.h/cpp` - Custom symbology

### Utility Classes
- `CoordinateConverter.h/cpp` - Coordinate system conversions
- `matrix.h/cpp` - Matrix operations for coordinate transformations

### Build and Test Files
- `CMakeLists.txt` - CMake build configuration
- `RadarDisplay.pro` - qmake project file
- `build.sh` - Automated build script
- `gps_test_sender.py` - GPS data test sender
- `simple_gps_demo_basic.cpp` - Console demo application

### Documentation
- `GPS_README.md` - Comprehensive usage documentation
- `PROJECT_SUMMARY.md` - This project overview
- `README.md` - Original project documentation

## 🚀 Key Features

### GPS Data Reception
- **JSON Format Support**: 
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

- **Binary Format Support**: Direct C structure transmission
- **Real-time Processing**: Sub-second GPS data processing
- **Multi-track Support**: Handle multiple simultaneous GPS sources

### Map Visualization
- **Professional Mapping**: QGIS-powered map display
- **Real-time Updates**: Live position tracking on map
- **Multiple Map Formats**: Support for various raster and vector formats
- **Interactive Navigation**: Pan, zoom, and map control features

### User Experience
- **Modern Qt5 Interface**: Clean, responsive GUI
- **Dockable Panels**: Customizable workspace layout
- **Keyboard Shortcuts**: Efficient operation
- **Real-time Data Display**: Live track information table

## 🛠️ Technical Architecture

### Multi-threaded Design
- **Main Thread**: GUI and map rendering
- **UDP Receiver Thread**: GPS data reception and parsing
- **Data Processing**: Coordinate conversion and track management

### Data Flow
1. GPS data received via UDP (port 2026)
2. Parsed as JSON or binary format
3. Validated and converted to display coordinates
4. Updated in central track database
5. Rendered on QGIS map canvas
6. Displayed in tracks information table

### Coordinate Systems
- **Input**: WGS84 GPS coordinates (lat/lon/alt)
- **Processing**: Local Cartesian coordinates for calculations
- **Display**: Map projection coordinates for QGIS rendering

## 📋 Requirements

### System Requirements
- Linux (Ubuntu/Debian recommended)
- Qt5.15.3 or later
- QGIS 3.x development libraries
- CMake 3.16+ or qmake
- C++17 compatible compiler

### Dependencies
```bash
# Qt5 packages
sudo apt-get install qt5-default qtbase5-dev qttools5-dev-tools
sudo apt-get install qtquickcontrols2-5-dev qml-module-qtquick-controls2
sudo apt-get install libqt5charts5-dev libqt5quickwidgets5

# QGIS packages
sudo apt-get install qgis-dev libqgis-dev
sudo apt-get install libgeos-c1v5 libproj-dev libspatialindex-dev
```

## 🔧 Build Instructions

### Quick Build
```bash
# Make build script executable
chmod +x build.sh

# Build with automatic system detection
./build.sh

# Or specify build system
./build.sh --cmake    # Use CMake
./build.sh --qmake    # Use qmake
```

### Manual Build (CMake)
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./bin/GpsRadarDisplay
```

### Manual Build (qmake)
```bash
qmake RadarDisplay.pro
make -j$(nproc)
./bin/RadarDisplay
```

## 🧪 Testing

### Console Demo (No Dependencies)
```bash
# Compile and run basic demo
g++ -std=c++17 simple_gps_demo_basic.cpp -o simple_gps_demo_basic -pthread
./simple_gps_demo_basic

# In another terminal, send test data
python3 gps_test_sender.py --mode static
```

### Full Application Testing
```bash
# Start the Qt application
./bin/RadarDisplay  # or ./build/bin/GpsRadarDisplay

# Send static test positions
python3 gps_test_sender.py --mode static

# Simulate moving track
python3 gps_test_sender.py --mode moving --track-id 100 --velocity 60 --heading 90 --duration 120
```

## 🎯 Usage Scenarios

### Real-time Vehicle Tracking
- Connect GPS-enabled vehicles to send position data
- Monitor multiple vehicles simultaneously
- Display tracks with different identities and colors

### Aircraft Monitoring
- Receive ADS-B or GPS data from aircraft
- Display flight paths and current positions
- Monitor altitude, heading, and velocity information

### Maritime Tracking
- Track ships and boats with GPS transponders
- Display vessel positions on nautical charts
- Monitor maritime traffic in real-time

### Search and Rescue Operations
- Track rescue teams and assets
- Coordinate multiple units on a common map
- Monitor progress and positions during operations

## 🔮 Future Enhancements

### Planned Features
- **Database Integration**: Store historical track data
- **Web Interface**: Browser-based monitoring dashboard
- **Alert System**: Configurable alerts for track events
- **Export Functions**: Export track data and maps
- **Mobile Support**: Android/iOS companion apps

### Potential Improvements
- **Performance Optimization**: Enhanced rendering for large track counts
- **Advanced Symbology**: Custom symbols for different track types
- **Network Protocols**: Support for additional data formats
- **Plugin System**: Extensible plugin architecture

## 📞 Support and Troubleshooting

### Common Issues
1. **QGIS Not Found**: Install QGIS development packages
2. **Qt5 Missing**: Install Qt5 development tools
3. **UDP Port Conflicts**: Check port availability and firewall settings
4. **Build Failures**: Verify all dependencies are installed

### Debug Information
- Application provides detailed console output
- GPS receiver logs all incoming data
- Map operations include error reporting
- Build script includes dependency checking

## 📄 License and Credits

This project builds upon:
- **Qt5 Framework** (LGPL License)
- **QGIS Libraries** (GPL License)
- **Standard C++ Libraries**

The GPS UDP receiver implementation and integration are original contributions designed specifically for real-time GPS tracking applications.

---

**Project Status**: ✅ Complete and Functional
**Last Updated**: 2025-10-05
**Version**: 1.0.0