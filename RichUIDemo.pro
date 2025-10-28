QT += core gui widgets qml quick quickcontrols2 charts

CONFIG += c++17

TARGET = RichUIDemo
TEMPLATE = app

SOURCES += \
    DemoApp.cpp \
    MainView_qml.cpp \
    qmlcache_loader.cpp

RESOURCES += \
    myRes.qrc \
    myRes_qmlcache.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target