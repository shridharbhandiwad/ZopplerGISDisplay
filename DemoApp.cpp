#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set the style for better UI appearance
    QQuickStyle::setStyle("Material");
    
    QQmlApplicationEngine engine;
    
    // Load the rich main view
    const QUrl url(QStringLiteral("qrc:/qml/RichMainView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    qDebug() << "Rich QML UI Demo Application Started";
    qDebug() << "Features:";
    qDebug() << "- Movable controls with drag and drop";
    qDebug() << "- Window management (minimize, maximize, close)";
    qDebug() << "- Real-time track table";
    qDebug() << "- Control factory for creating new controls";
    qDebug() << "- Multiple control types: Gauge, Chart, Slider, Display, Button Panel";
    
    return app.exec();
}