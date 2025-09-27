#include "cmapmainwindow.h"
#include "MapDisplay/cgismapcontroller.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDir>
#include "qgsapplication.h"
#include "MapDisplay/cgismapcontroller.h"
#include "globalmacros.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QgsApplication::initQgis();

    qDebug()<<"RADAR DISPLAY";
    qDebug()<<"APP VERSION :"<<APP_VERSION;
    qDebug()<<"APP BUILT DATE :"<<APP_BUILD_DATE;

     CMapMainWindow w;

     w.showMaximized();

    return app.exec();
}
