#include "cmapcanvas.h"
#include <qgsvectorlayer.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <qgsrectangle.h>
#include <QDir>
#include <QFileInfoList>
#include <qgscoordinatetransformcontext.h>
#include <qgsfillsymbol.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsfillsymbol.h>
#include <qgsrenderer.h>
#include <qgsproject.h>
#include <qgssymbollayerutils.h>  // sometimes contains symbol layer helpers
#include <qgsfillsymbollayer.h> // *If* it exists in your build
#include  "customgradiantfillsymbollayer.h"
#include <qgslinesymbollayer.h>
#include"globalmacros.h"
#include "cdatawarehouse.h"
#include  <QFileInfo>
#include  <QProcess>

CMapCanvas::CMapCanvas(QWidget *parent) : QgsMapCanvas(parent),
    _m_ppiLayer(nullptr),_m_trackLayer(nullptr)
{
    QgsRectangle fixedWorldExtent(-180.0, -90.0, 180.0, 90.0);

    // Add padding (e.g., 10% of width/height)
    double padX = fixedWorldExtent.width() * 0.1;
    double padY = fixedWorldExtent.height() * 0.1;

    mWorldExtentPadded = QgsRectangle(
        fixedWorldExtent.xMinimum() - padX,
        fixedWorldExtent.yMinimum() - padY,
        fixedWorldExtent.xMaximum() + padX,
        fixedWorldExtent.yMaximum() + padY);

    setCanvasColor(QColor("#050D1A"));
    enableAntiAliasing(true);
    setRenderFlag(true);
    freeze(false);
    QgsCoordinateReferenceSystem crs("EPSG:4326"); // WGS 84 (lat/lon)
    setDestinationCrs(crs);
}

void CMapCanvas::Initialize() {

    _loadRasterMaps();
    _loadVectorMaps();
    _loadLayers();
    zoomToFullExtent();
    refresh();
}

void CMapCanvas::importRasterMap(QString inputPath) {
    convertAndCacheRaster(inputPath);
}

void CMapCanvas::convertAndCacheRaster(const QString inputPath)
{
    QFileInfo info(inputPath);
    QString cacheDir = QDir::currentPath() + "/../maps/RasterRepository";
    QDir().mkpath(cacheDir);

    m_outputPath = cacheDir + "/"+info.baseName()+".TIFF";
    m_inputPath = inputPath;

    QDir().mkpath(QFileInfo(m_outputPath).absolutePath());

    if (m_progressDialog)
        delete m_progressDialog;

    m_progressDialog = new QProgressDialog("Preparing raster...", "Cancel", 0, 100, this);
    m_progressDialog->setWindowModality(Qt::ApplicationModal);
    m_progressDialog->setWindowTitle("Raster Processing");
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->setAutoClose(false);
    m_progressDialog->setAutoReset(false);
    m_progressDialog->show();

    QTimer::singleShot(100, this, &CMapCanvas::startGdalTranslate);

}

void CMapCanvas::startGdalTranslate()
{
    if (m_translateProcess)
        delete m_translateProcess;

    m_translateProcess = new QProcess(this);
    connect(m_translateProcess, &QProcess::readyReadStandardOutput, this, &CMapCanvas::handleGdalStdout);
    connect(m_translateProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::startGdalAddo);

    QString cmd = QString("gdal_translate -of GTiff -co TILED=YES -co COMPRESS=DEFLATE \"%1\" \"%2\"")
                  .arg(m_inputPath, m_outputPath);
    //qDebug() << "Running gdal_translate:" << cmd;

    m_translateProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_translateProcess->start(cmd);
}

void CMapCanvas::startGdalAddo()
{
    if (m_addoProcess)
        delete m_addoProcess;

    m_addoProcess = new QProcess(this);
    connect(m_addoProcess, &QProcess::readyReadStandardOutput, this, &CMapCanvas::handleGdalStdout);
    connect(m_addoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CMapCanvas::loadCachedAfterProcessing);

    QString cmd = QString("gdaladdo -r average \"%1\" 2 4 8 16 32 64").arg(m_outputPath);
    //qDebug() << "Running gdaladdo:" << cmd;

    m_addoProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_addoProcess->start(cmd);
}

void CMapCanvas::handleGdalStdout()
{
    QProcess* proc = qobject_cast<QProcess*>(sender());
    if (!proc) return;

    QByteArray output = proc->readAllStandardOutput();
    QString text = QString::fromUtf8(output);
    //qDebug()<<text;
    //m_progressDialog->setLabelText(text);

    QRegularExpression re(R"((\d+))");  // Matches one or more digits

    QRegularExpressionMatch match = re.match(text);
    if (match.hasMatch()) {
        QString numberStr = match.captured(1); // First capturing group
        int number = numberStr.toInt();
        m_progressDialog->setValue(number);
    }


//    // GDAL prints progress like: "0...10...20...30..."
//    static QRegularExpression re(R"((\d{1,3})\.\.\.)");
//    QRegularExpressionMatchIterator i = re.globalMatch(text);
//    int lastValue = -1;

//    while (i.hasNext()) {
//        QRegularExpressionMatch match = i.next();
//        lastValue = match.captured(1).toInt();
//    }

//    if (lastValue >= 0 && lastValue <= 100 && m_progressDialog) {
//        m_progressDialog->setValue(lastValue);
//    }
}

void CMapCanvas::loadCachedAfterProcessing()
{
    if (m_progressDialog) {
        m_progressDialog->setValue(100);
        m_progressDialog->close();
    }

    loadRasterFile(m_outputPath);
}

void CMapCanvas::loadRasterFile( QString rasterPath) {

    QFileInfo fileInfo(rasterPath);
    if (!fileInfo.exists()) {
        qDebug() << "Raster file not found:" << rasterPath;
        return;
    }


    // Create raster layer
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(rasterPath, fileInfo.completeBaseName(),"gdal");

    if (!rasterLayer->isValid()) {
        qDebug() << "Invalid raster layer: " << rasterPath;
        delete rasterLayer;
        return;
    }

    //rasterLayer->setcac(true);

    // Add to QGIS project and canvas

    QgsProject::instance()->addMapLayer(rasterLayer);
    QList<QgsMapLayer*> alllayers = layers();
    alllayers.insert(0, rasterLayer);  // Background (lowest)
    setLayers({ alllayers });
    //setDestinationCrs(wgs84); // Important for canvas reprojectionsetDestinationCrs(wgs84); // Important for canvas reprojection
    //setExtent(rasterLayer->extent());
    refresh();

    //qDebug() << "Loaded raster with world file:" << rasterPath<<rasterLayer->extent();
}

void CMapCanvas::_loadRasterMaps() {

    QString directoryPath = "../maps/RasterRepository/";
    QDir dir(directoryPath);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << directoryPath;
        return;
    }

    QStringList filters;
    filters << "*.TIFF"<<"*.tiff";

    QFileInfoList rasterFiles = dir.entryInfoList(filters, QDir::Files);

    if (rasterFiles.isEmpty()) {
        qDebug() << "No raster files found in directory:" << directoryPath;
        return;
    }

    for (const QFileInfo &fileInfo : rasterFiles) {
        QString filePath = fileInfo.absoluteFilePath();
        //qDebug() << "Loading shapefile:" << filePath;
        loadRasterFile(filePath);
    }
}

void CMapCanvas::_loadVectorMaps() {

    QString directoryPath = "../maps/ShpRepository/";
    QDir dir(directoryPath);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << directoryPath;
        return;
    }

    QStringList filters;
    filters << "*.shp";

    QFileInfoList shpFiles = dir.entryInfoList(filters, QDir::Files);

    if (shpFiles.isEmpty()) {
        qDebug() << "No shapefiles found in directory:" << directoryPath;
        return;
    }

    for (const QFileInfo &fileInfo : shpFiles) {
        QString filePath = fileInfo.absoluteFilePath();
        //qDebug() << "Loading shapefile:" << filePath;
        loadShapeFile(filePath);
    }
}

void CMapCanvas::_loadLayers() {

    QPointF radarPos = CDataWarehouse::getInstance()->getRadarPos();
    _m_ppiLayer = new CPPILayer(this);
    _m_ppiLayer->setCenter(QgsPointXY(radarPos.x(), radarPos.y())); // or use dynamic source like GPS
    _m_ppiLayer->setMaxRange(8000);
    _m_ppiLayer->setRangeRingCount(4);
    _m_ppiLayer->setAzimuthStep(45);
    _m_ppiLayer->show();

    _m_trackLayer = new CTrackLayer(this);
    _m_trackLayer->show();
}

void CMapCanvas::loadShapeFile(const QString &shpPath)
{
    QgsVectorLayer *layer = new QgsVectorLayer(shpPath, QFileInfo(shpPath).baseName(), "ogr");

    if (!layer->isValid()) {
        qDebug() << "Layer failed to load: " << shpPath;
        delete layer;  // Avoid memory leak
        return;
    }

    QString baseName = QFileInfo(shpPath).baseName().toLower();

        QgsGradientFillSymbolLayer *gradientLayer = new QgsGradientFillSymbolLayer();
//        gradientLayer->setColor(QColor("#008888"));     // Start color (cyan)
//        gradientLayer->setColor2(QColor("#005555"));    // End color (dark blue)
        gradientLayer->setColor(QColor("#000000"));     // Start color (cyan)
        gradientLayer->setColor2(QColor("#505050"));    // End color (dark blue)
        gradientLayer->setGradientSpread(Qgis::GradientSpread::Pad); // Optional: for better visuals
        // Set gradient type (Linear or Radial)
        gradientLayer->setGradientType(Qgis::GradientType::Radial);
        // Set coordinate mode
        gradientLayer->setCoordinateMode(Qgis::SymbolCoordinateReference::Viewport);

        // Optionally adjust reference points (between 0 and 1)
        gradientLayer->setReferencePoint1(QPointF(0.0, 0.0));
        gradientLayer->setReferencePoint2(QPointF(1.0, 1.0));

        QgsFillSymbol *symbol = new QgsFillSymbol();
        symbol->deleteSymbolLayer(0);  // Remove the default simple fill
        symbol->appendSymbolLayer(gradientLayer);


        // Now add a stroke layer for outline
        QgsSimpleLineSymbolLayer *outlineLayer = new QgsSimpleLineSymbolLayer();
        outlineLayer->setColor(QColor("#AA66FF"));  // Stroke color (blue)
        outlineLayer->setWidth(0.2);                 // Stroke width in map units (adjust as needed)

        symbol->appendSymbolLayer(outlineLayer);

        QgsSingleSymbolRenderer *renderer = dynamic_cast<QgsSingleSymbolRenderer *>(layer->renderer());
        if (renderer)
        {
            renderer->setSymbol(symbol);
        }
        else
        {
            // If it's not already a single symbol renderer, create a new one
            renderer = new QgsSingleSymbolRenderer(symbol);
            layer->setRenderer(renderer);
        }

        // Assign it to your vector layer
        //layer->renderer()->setSymbol(symbol);

        // Refresh the layer rendering
        layer->triggerRepaint();

    QgsProject::instance()->addMapLayer(layer);

    // Keep existing layers and add the new one
    QList<QgsMapLayer *> currentLayers = layers();
    currentLayers.append(layer);
    setLayers(currentLayers);

    // Optional: Zoom to the extent of the first layer loaded
//    if (currentLayers.size() == 1) {
//        setExtent(layer->extent());
//        refresh();
//    }
}

void CMapCanvas::mapHome() {

    if ( _m_ppiLayer != nullptr ) {
        QgsRectangle rectExtent = _m_ppiLayer->boundingRectWorld();
        setExtent(rectExtent);
        refresh();
    }
}

void CMapCanvas::zoomBy(double factor)
{
    QgsRectangle currentExtent = extent();
    QgsPointXY center = currentExtent.center();

    double newWidth = currentExtent.width() * factor;
    double newHeight = currentExtent.height() * factor;

    QgsRectangle newExtent(
        center.x() - newWidth / 2,
        center.y() - newHeight / 2,
        center.x() + newWidth / 2,
        center.y() + newHeight / 2
    );

    if ( factor > 1 ) {
        if (newExtent.width() > mWorldExtentPadded.width() ||
            newExtent.height() > mWorldExtentPadded.height())
        {
            newExtent = mWorldExtentPadded;
        }
    }

    setExtent(newExtent);
    refresh();
}

void CMapCanvas::panCanvas(double dx, double dy)
{
    QgsRectangle currentExtent = extent();
    currentExtent.setXMinimum(currentExtent.xMinimum() + dx);
    currentExtent.setXMaximum(currentExtent.xMaximum() + dx);
    currentExtent.setYMinimum(currentExtent.yMinimum() + dy);
    currentExtent.setYMaximum(currentExtent.yMaximum() + dy);

    setExtent(currentExtent);
    refresh();
}

void CMapCanvas::wheelEvent(QWheelEvent *event)
{
//    const double zoomFactor = 1.1;
//    if (event->angleDelta().y() > 0)
//        zoomByFactor(1 / zoomFactor); // Zoom in
//    else
//        zoomByFactor(zoomFactor);     // Zoom out

    const double zoomFactor = 1.1;
    double factor = (event->angleDelta().y() > 0) ? (1.0 / zoomFactor) : zoomFactor;

    // Get the position of the mouse in map coordinates
    QPoint mousePos = event->pos();
    qDebug()<<mousePos;
    QgsPointXY mapPointBeforeZoom = getCoordinateTransform()->toMapCoordinates(mousePos.x(), mousePos.y());

    // Zoom normally
    zoomBy(factor);

    // Get the new map coordinates of the same screen point
    QgsPointXY mapPointAfterZoom = getCoordinateTransform()->toMapCoordinates(mousePos.x(), mousePos.y());

    // Compute how much the map moved due to zoom
    double dx = mapPointBeforeZoom.x() - mapPointAfterZoom.x();
    double dy = mapPointBeforeZoom.y() - mapPointAfterZoom.y();

    // Pan the canvas to offset the difference — this keeps the cursor point stable
    panCanvas(dx, dy);
}

void CMapCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mLastMousePos = event->pos();
        mPanning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CMapCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (mPanning) {
//        QPoint delta = event->pos() - mLastMousePos;
//        panCanvas(-delta.x(), delta.y());
//        mLastMousePos = event->pos();

        QPoint delta = event->pos() - mLastMousePos;

        double dx = -delta.x() * mapSettings().mapUnitsPerPixel();
        double dy = delta.y() * mapSettings().mapUnitsPerPixel();  // Negative because Y axis is inverted in screen coords

        panCanvas(dx, dy);  // Now using proper map units
        mLastMousePos = event->pos();
    }

    // Convert mouse screen position to map coordinates
    QgsPointXY mapPoint = getCoordinateTransform()->toMapCoordinates(event->pos().x(), event->pos().y());

    // Convert to geographic (lat/lon) using the destination CRS (assumed WGS84)
    QgsCoordinateTransform transform(mapSettings().destinationCrs(), QgsCoordinateReferenceSystem("EPSG:4326"), QgsProject::instance());
    QgsPointXY geoPoint = transform.transform(mapPoint);

    double alt = 0.0;  // Optional: add altitude logic here if needed

    emit signalMouseRead(QString("Lon : %1°    Lat : %2° ").arg(geoPoint.x(),11,'f',6).arg(geoPoint.y(),10,'f',6));
}

void CMapCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mPanning = false;
        setCursor(Qt::ArrowCursor);
    }
}

void CMapCanvas::keyPressEvent(QKeyEvent *event)
{
    QgsRectangle extent = this->extent();
    double moveX = extent.width() * 0.1; // 10% move

    switch (event->key()) {
    case Qt::Key_Left:
        panCanvas(-moveX, 0);
        break;
    case Qt::Key_Right:
        panCanvas(moveX, 0);
        break;
    case Qt::Key_Up:
        panCanvas(0, moveX);
        break;
    case Qt::Key_Down:
        panCanvas(0, -moveX);
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:  // Shift + '=' is usually '+'
        zoomBy(1 / 1.1);  // Zoom in
        break;    case Qt::Key_Underscore:
    case Qt::Key_Minus:
        zoomBy(1.1);      // Zoom out
        break;
    default:
        QgsMapCanvas::keyPressEvent(event); // Default handling
        break;
    }
}
