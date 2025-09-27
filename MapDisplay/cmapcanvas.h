#ifndef CMAPCANVAS_H
#define CMAPCANVAS_H

#include <QObject>
#include <QWidget>
#include "qgsmapcanvas.h"
#include <QPoint>
#include "cppilayer.h"
#include "ctracklayer.h"
#include "qgsrasterlayer.h"
#include <QProcess>
#include <QProgressDialog>


class CMapCanvas : public QgsMapCanvas
{
    Q_OBJECT

public:
    explicit CMapCanvas(QWidget *parent = nullptr);

    void Initialize();

    void mapHome();

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void loadRasterFile(QString rasterPath);
    void importRasterMap(QString inputPath);
private:

    QProcess* m_translateProcess = nullptr;
    QProcess* m_addoProcess = nullptr;
    QString m_inputPath;
    QString m_outputPath;

    QProgressDialog* m_progressDialog = nullptr;

    QgsRectangle mWorldExtentPadded;
    void zoomBy(double factor);
    void panCanvas(double dx, double dy);

    QPoint mLastMousePos;
    bool mPanning = false;

    void _loadVectorMaps();
    void loadShapeFile(const QString &shpPath);

    CPPILayer *_m_ppiLayer;
    CTrackLayer *_m_trackLayer;
    void _loadLayers();
    void convertAndCacheRaster(const QString inputPath);
    void _loadRasterMaps();
signals:
    void signalMouseRead(QString);
public slots:
private slots:
    void startGdalTranslate();
    void startGdalAddo();
    void handleGdalStdout();
    void loadCachedAfterProcessing();
};

#endif // CMAPCANVAS_H
