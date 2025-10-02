#-------------------------------------------------
#
# Project created by QtCreator 2025-06-02T22:24:56
#
#-------------------------------------------------

QT       += core gui widgets xml network concurrent qml quick quickwidgets quickcontrols2 charts


OBJECTS_DIR = ../obj/OBJ
MOC_DIR = ../obj/MOC
UI_DIR = ../obj/UI
RCC_DIR = ../obj/RCC

TARGET = ../bin/RadarDisplay
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

# QGIS includes
INCLUDEPATH += /usr/include/qgis

# QGIS libraries
LIBS += -lqgis_core -lqgis_gui -lqgis_analysis

# QGIS dependencies (if needed)
LIBS += -lgeos_c -lproj -lspatialindex

SOURCES += \
        CoordinateConverter.cpp \
        DemoApp.cpp \
        MainView_qml.cpp \
        MapDisplay/cgismapcontroller.cpp \
        MapDisplay/cmapdisplay.cpp \
        MapDisplay/cmapcanvas.cpp \
        MapDisplay/cppilayer.cpp \
        MapDisplay/csearchbeamlayer.cpp \
        MapDisplay/ctracklayer.cpp \
        MapDisplay/customgradiantfillsymbollayer.cpp \
        cdatawarehouse.cpp \
        cmapmainwindow.cpp \
        cudpreceiver.cpp \
        main.cpp \
        matrix.cpp \
        qmlcache_loader.cpp

HEADERS += \
        CoordinateConverter.h \
        MapDisplay/cgismapcontroller.h \
        MapDisplay/cmapcanvas.h \
        MapDisplay/cmapdisplay.h \
        MapDisplay/cppilayer.h \
        MapDisplay/csearchbeamlayer.h \
        MapDisplay/ctracklayer.h \
        MapDisplay/customgradiantfillsymbollayer.h \
        cdatawarehouse.h \
        cmapmainwindow.h \
        cudpreceiver.h \
        globalmacros.h \
        globalstructs.h \
        matrix.h

FORMS += \
        cmapmainwindow.ui \
        MapDisplay/cmapdisplay.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    myRes.qrc \
    myRes_qmlcache.qrc
