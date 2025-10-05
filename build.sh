#!/bin/bash

# GPS Radar Display Build Script
# Supports both CMake and qmake build systems

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check dependencies
check_dependencies() {
    print_status "Checking dependencies..."
    
    local missing_deps=()
    
    # Check Qt5
    if ! command_exists qmake; then
        missing_deps+=("qt5-default qtbase5-dev")
    fi
    
    # Check CMake (optional)
    if ! command_exists cmake; then
        print_warning "CMake not found - qmake build only"
    fi
    
    # Check QGIS development files
    if [ ! -f "/usr/include/qgis/qgsapplication.h" ] && [ ! -f "/usr/local/include/qgis/qgsapplication.h" ]; then
        missing_deps+=("qgis-dev libqgis-dev")
    fi
    
    # Check for required libraries
    if ! pkg-config --exists Qt5Core Qt5Widgets Qt5Network; then
        missing_deps+=("qtbase5-dev qttools5-dev-tools")
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        print_error "Missing dependencies detected!"
        echo "Please install the following packages:"
        printf '  sudo apt-get install %s\n' "${missing_deps[@]}"
        echo ""
        echo "Full installation command:"
        echo "  sudo apt-get update"
        echo "  sudo apt-get install qt5-default qtbase5-dev qttools5-dev-tools"
        echo "  sudo apt-get install qtquickcontrols2-5-dev qml-module-qtquick-controls2"
        echo "  sudo apt-get install libqt5charts5-dev libqt5quickwidgets5"
        echo "  sudo apt-get install qgis-dev libqgis-dev"
        echo "  sudo apt-get install libgeos-c1v5 libproj-dev libspatialindex-dev"
        exit 1
    fi
    
    print_success "All dependencies found"
}

# Function to build with CMake
build_cmake() {
    print_status "Building with CMake..."
    
    # Create build directory
    if [ -d "build" ]; then
        print_status "Cleaning existing build directory..."
        rm -rf build
    fi
    
    mkdir build
    cd build
    
    # Configure
    print_status "Configuring project..."
    cmake .. || {
        print_error "CMake configuration failed"
        exit 1
    }
    
    # Build
    print_status "Building project..."
    make -j$(nproc) || {
        print_error "Build failed"
        exit 1
    }
    
    cd ..
    
    if [ -f "build/bin/GpsRadarDisplay" ]; then
        print_success "CMake build completed successfully!"
        print_status "Executable: build/bin/GpsRadarDisplay"
    else
        print_error "Build completed but executable not found"
        exit 1
    fi
}

# Function to build with qmake
build_qmake() {
    print_status "Building with qmake..."
    
    # Clean previous build
    if [ -f "Makefile" ]; then
        print_status "Cleaning previous build..."
        make clean 2>/dev/null || true
        rm -f Makefile
    fi
    
    # Create output directories
    mkdir -p bin obj/OBJ obj/MOC obj/UI obj/RCC
    
    # Generate Makefile
    print_status "Generating Makefile..."
    qmake RadarDisplay.pro || {
        print_error "qmake failed"
        exit 1
    }
    
    # Build
    print_status "Building project..."
    make -j$(nproc) || {
        print_error "Build failed"
        exit 1
    }
    
    if [ -f "bin/RadarDisplay" ]; then
        print_success "qmake build completed successfully!"
        print_status "Executable: bin/RadarDisplay"
    else
        print_error "Build completed but executable not found"
        exit 1
    fi
}

# Function to make test sender executable
setup_test_sender() {
    if [ -f "gps_test_sender.py" ]; then
        chmod +x gps_test_sender.py
        print_success "GPS test sender is ready"
    fi
}

# Main script
main() {
    echo "================================================"
    echo "GPS Radar Display - Build Script"
    echo "================================================"
    
    # Parse command line arguments
    BUILD_SYSTEM="auto"
    SKIP_DEPS=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --cmake)
                BUILD_SYSTEM="cmake"
                shift
                ;;
            --qmake)
                BUILD_SYSTEM="qmake"
                shift
                ;;
            --skip-deps)
                SKIP_DEPS=true
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [OPTIONS]"
                echo ""
                echo "Options:"
                echo "  --cmake      Force CMake build system"
                echo "  --qmake      Force qmake build system"
                echo "  --skip-deps  Skip dependency checking"
                echo "  --help, -h   Show this help message"
                echo ""
                echo "Default: Auto-detect best build system"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    # Check dependencies unless skipped
    if [ "$SKIP_DEPS" = false ]; then
        check_dependencies
    fi
    
    # Determine build system
    if [ "$BUILD_SYSTEM" = "auto" ]; then
        if command_exists cmake; then
            BUILD_SYSTEM="cmake"
            print_status "Auto-selected CMake build system"
        else
            BUILD_SYSTEM="qmake"
            print_status "Auto-selected qmake build system"
        fi
    fi
    
    # Build the project
    case $BUILD_SYSTEM in
        cmake)
            build_cmake
            ;;
        qmake)
            build_qmake
            ;;
        *)
            print_error "Invalid build system: $BUILD_SYSTEM"
            exit 1
            ;;
    esac
    
    # Setup test utilities
    setup_test_sender
    
    echo ""
    echo "================================================"
    print_success "Build completed successfully!"
    echo "================================================"
    echo ""
    echo "Next steps:"
    echo "1. Run the application:"
    if [ "$BUILD_SYSTEM" = "cmake" ]; then
        echo "   ./build/bin/GpsRadarDisplay"
    else
        echo "   ./bin/RadarDisplay"
    fi
    echo ""
    echo "2. Test with GPS data:"
    echo "   python3 gps_test_sender.py --mode static"
    echo "   python3 gps_test_sender.py --mode moving --velocity 60"
    echo ""
    echo "3. Read GPS_README.md for detailed usage instructions"
    echo ""
}

# Run main function
main "$@"