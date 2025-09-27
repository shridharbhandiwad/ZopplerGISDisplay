#include "cppilayer.h"
#include <QtMath>
#include "globalmacros.h"

CPPILayer::CPPILayer(QgsMapCanvas *canvas) : QgsMapCanvasItem(canvas),
    _m_canvas(canvas), _m_searchbeamLayer(nullptr)
{
    setZValue(100);

    _m_searchbeamLayer = new CSearchBeamLayer(canvas);
}

QgsMapCanvas *CPPILayer::canvas() {
    return _m_canvas;
}

void CPPILayer::setCenter(const QgsPointXY &center)
{
    if ( _m_searchbeamLayer != nullptr ) {
        _m_searchbeamLayer->setCenter(center);
    }
    m_center.set(center.x(),center.y());
    updatePosition();
}

void CPPILayer::setMaxRange(double rangeMeters)
{
    if ( _m_searchbeamLayer != nullptr ) {
        _m_searchbeamLayer->setMaxRange(rangeMeters);
    }
    m_maxRange = rangeMeters;
    update();
}

void CPPILayer::setRangeRingCount(int count)
{
    m_ringCount = count;
    update();
}

void CPPILayer::setAzimuthStep(int degrees)
{
    m_azimuthStep = degrees;
    update();
}

void CPPILayer::updatePosition()
{
    const QgsMapToPixel &mapToPixel = canvas()->mapSettings().mapToPixel();
    m_centerScreen = mapToPixel.transform(m_center).toQPointF();
    //qDebug()<<m_centerScreen.x()<<m_centerScreen.y();
    update();
}

QRectF CPPILayer::boundingRect() const
{
    // Large enough to contain all range rings
    const QgsMapCanvas *canvass = _m_canvas;
    double radiusPixels = m_maxRange / canvass->mapSettings().mapUnitsPerPixel();
    return QRectF(m_centerScreen.x() - radiusPixels, m_centerScreen.y() - radiusPixels,
                  radiusPixels * 2, radiusPixels * 2);
}

QgsRectangle CPPILayer::boundingRectWorld() const
{
    // Convert meters to degrees (rough approximation)
    double metersPerDegreeLat = 111132.0;
    double metersPerDegreeLon = 111320.0 * std::cos(qDegreesToRadians(m_center.y()));

    double deltaLon = (m_maxRange * 1.1) / metersPerDegreeLon;
    double deltaLat = (m_maxRange * 1.1) / metersPerDegreeLat;

    // Create a rectangle around the center with size based on range
    QgsRectangle extent(
        m_center.x() - deltaLon,
        m_center.y() - deltaLat,
        m_center.x() + deltaLon,
        m_center.y() + deltaLat
    );

    return extent;

}

void CPPILayer::paint(QPainter *painter)
{
    if (!canvas()) return;

    double metersPerDegreeLat = 111132.0; // avg for latitude
    double metersPerDegreeLon = 111320.0 * std::cos(qDegreesToRadians(m_center.y())); // depends on latitude


    painter->setRenderHint(QPainter::Antialiasing, true);
    double radiusStep = m_maxRange / m_ringCount;
    double pixelPerDegree = 1.0 / canvas()->mapUnitsPerPixel();

    if ( pixelPerDegree < PPI_VISIBLE_THRESHOLD) {
        return;
    }

    double pixelPerMeterX = pixelPerDegree / metersPerDegreeLon;
    double pixelPerMeterY = pixelPerDegree / metersPerDegreeLat;

    double avgPixelPerMeter = (pixelPerMeterX + pixelPerMeterY) / 2.0;

    //qDebug()<<avgPixelPerMeter<<pixelPerDegree;

    painter->setFont(QFont("century",10,80,false));

    // Draw range rings
    QPen ringPen(Qt::cyan, 1.5);
    painter->setPen(ringPen);
    for (int i = 1; i <= m_ringCount; ++i) {
        double r = radiusStep * i * avgPixelPerMeter;
        painter->drawEllipse(m_centerScreen, r, r);
        if ( pixelPerDegree > TEXT_VISIBLE_THRESHOLD ) {
            QString rangeLabel = QString::number((radiusStep * i)/1000.0) + " km";
            QPointF labelPos(m_centerScreen.x() + 5, m_centerScreen.y() - r - 5);
            painter->drawText(labelPos, rangeLabel);
        }
    }

    // Draw azimuth lines
    //QPen azimuthPen(Qt::cyan, 1.5);
    //painter->setPen(azimuthPen);

    for (int angle = 0; angle < 360; angle += m_azimuthStep) {
        double radians = qDegreesToRadians(static_cast<double>(angle));
        double x = m_centerScreen.x() + std::cos(radians) * m_maxRange * pixelPerMeterX;
        double y = m_centerScreen.y() - std::sin(radians) * m_maxRange * pixelPerMeterY;
        painter->drawLine(m_centerScreen, QPointF(x, y));

        if ( pixelPerDegree > TEXT_VISIBLE_THRESHOLD ) {
            int az = ((360 - angle) + 90) % 360;
            if ( az > 0 ) {
                // Azimuth marker text
                QString azText = QString::number(az) + "°";
                QPointF textPos(
                    m_centerScreen.x() + std::cos(radians) * (m_maxRange * 1.05) * pixelPerMeterX,
                    m_centerScreen.y() - std::sin(radians) * (m_maxRange * 1.05) * pixelPerMeterY
                );
                QRectF boundingBox(textPos.x() - 20, textPos.y() - 10, 40, 20);
                painter->drawText(boundingBox, Qt::AlignCenter, azText);
            }
        }
    }

    // Optionally draw center marker
    painter->setPen(Qt::red);    painter->drawEllipse(m_centerScreen, 3, 3);
}
