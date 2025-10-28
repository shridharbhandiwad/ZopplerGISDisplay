cmake_minimum_required(VERSION 3.16)
project(GpsRadarDisplay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt5 components
find_package(Qt5 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Network 
    Xml 
    Concurrent 
    Qml 
    Quick 
    QuickWidgets 
    QuickControls2 
    Charts
)

# Find QGIS
find_path(QGIS_INCLUDE_DIR qgsapplication.h
    PATHS
    /usr/include/qgis
    /usr/local/include/qgis
    /opt/qgis/include
)

find_library(QGIS_CORE_LIBRARY qgis_core
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/qgis/lib
)

find_library(QGIS_GUI_LIBRARY qgis_gui
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/qgis/lib
)

find_library(QGIS_ANALYSIS_LIBRARY qgis_analysis
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/qgis/lib
)

# Check if QGIS was found
if(NOT QGIS_INCLUDE_DIR OR NOT QGIS_CORE_LIBRARY OR NOT QGIS_GUI_LIBRARY)
    message(FATAL_ERROR "QGIS libraries not found. Please install QGIS development packages.")
endif()

# Find additional dependencies
find_library(GEOS_C_LIBRARY geos_c)
find_library(PROJ_LIBRARY proj)
find_library(SPATIALINDEX_LIBRARY spatialindex)

# Set up the executable
set(SOURCES
    main.cpp
    cmapmainwindow.cpp
    cdatawarehouse.cpp
    cudpreceiver.cpp
    cgpsudpreceiver.cpp
    CoordinateConverter.cpp
    matrix.cpp
    MapDisplay/cmapcanvas.cpp
    MapDisplay/cppilayer.cpp
    MapDisplay/csearchbeamlayer.cpp
    MapDisplay/ctracklayer.cpp
    MapDisplay/customgradiantfillsymbollayer.cpp
)

set(HEADERS
    cmapmainwindow.h
    cdatawarehouse.h
    cudpreceiver.h
    cgpsudpreceiver.h
    CoordinateConverter.h
    matrix.h
    globalmacros.h
    globalstructs.h
    MapDisplay/cmapcanvas.h
    MapDisplay/cppilayer.h
    MapDisplay/csearchbeamlayer.h
    MapDisplay/ctracklayer.h
    MapDisplay/customgradiantfillsymbollayer.h
)

set(UI_FILES
    cmapmainwindow.ui
)

set(RESOURCE_FILES
    myRes.qrc
)

# Process Qt files
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
qt5_wrap_ui(UI_SOURCES ${UI_FILES})
qt5_add_resources(QRC_SOURCES ${RESOURCE_FILES})

# Create the executable
add_executable(GpsRadarDisplay
    ${SOURCES}
    ${MOC_SOURCES}
    ${UI_SOURCES}
    ${QRC_SOURCES}
)

# Include directories
target_include_directories(GpsRadarDisplay PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/MapDisplay
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QGIS_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(GpsRadarDisplay
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::QuickControls2
    Qt5::Charts
    ${QGIS_CORE_LIBRARY}
    ${QGIS_GUI_LIBRARY}
    ${QGIS_ANALYSIS_LIBRARY}
)

# Link additional dependencies if found
if(GEOS_C_LIBRARY)
    target_link_libraries(GpsRadarDisplay ${GEOS_C_LIBRARY})
endif()

if(PROJ_LIBRARY)
    target_link_libraries(GpsRadarDisplay ${PROJ_LIBRARY})
endif()

if(SPATIALINDEX_LIBRARY)
    target_link_libraries(GpsRadarDisplay ${SPATIALINDEX_LIBRARY})
endif()

# Compiler definitions
target_compile_definitions(GpsRadarDisplay PRIVATE
    QT_DEPRECATED_WARNINGS
)

# Set output directory
set_target_properties(GpsRadarDisplay PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS GpsRadarDisplay
    RUNTIME DESTINATION bin
)