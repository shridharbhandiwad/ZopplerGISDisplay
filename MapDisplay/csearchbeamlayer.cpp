#include "csearchbeamlayer.h"
#include <qgsmapsettings.h>
#include <qgsmaptopixel.h>
#include <QPainter>
#include <QtMath>
#include "globalmacros.h"

CSearchBeamLayer::CSearchBeamLayer(QgsMapCanvas *canvas)
    : QgsMapCanvasItem(canvas), _m_canvas(canvas)
{
    setZValue(102);

    connect(&_animationTimer, &QTimer::timeout, this, [this]() {
           _sweepAngle += 2.0;
           if (_sweepAngle >= 360.0) _sweepAngle = 0.0;
           update();  // Trigger repaint
       });
       _animationTimer.start(10);  // ~33 FPS
}

QgsMapCanvas *CSearchBeamLayer::canvas() {
    return _m_canvas;
}

void CSearchBeamLayer::setCenter(const QgsPointXY &center)
{
    m_center.set(center.x(),center.y());
    updatePosition();
}

void CSearchBeamLayer::setMaxRange(double rangeMeters)
{
    m_maxRange = rangeMeters;
    update();
}

void CSearchBeamLayer::updatePosition()
{
    const QgsMapToPixel &mapToPixel = canvas()->mapSettings().mapToPixel();
    m_centerScreen = mapToPixel.transform(m_center).toQPointF();
    //qDebug()<<m_centerScreen.x()<<m_centerScreen.y();
    update();
}

QRectF CSearchBeamLayer::boundingRect() const
{
    // Large enough to contain all range rings
    const QgsMapCanvas *canvass = _m_canvas;
    double radiusPixels = m_maxRange / canvass->mapSettings().mapUnitsPerPixel();
    return QRectF(m_centerScreen.x() - radiusPixels, m_centerScreen.y() - radiusPixels,
                  radiusPixels * 2, radiusPixels * 2);
}


void CSearchBeamLayer::paint(QPainter *painter)
{
    if (!canvas()) return;

    double metersPerDegreeLat = 111132.0; // avg for latitude
    double metersPerDegreeLon = 111320.0 * std::cos(qDegreesToRadians(m_center.y())); // depends on latitude


    painter->setRenderHint(QPainter::Antialiasing, true);

    double pixelPerDegree = 1.0 / canvas()->mapUnitsPerPixel();

    if ( pixelPerDegree < PPI_VISIBLE_THRESHOLD) {
        return;
    }

    double pixelPerMeterX = pixelPerDegree / metersPerDegreeLon;
    double pixelPerMeterY = pixelPerDegree / metersPerDegreeLat;

    // Draw radar sweep beam
    double sweepWidth = 60.0; // in degrees
    double startAngle = (360 - _sweepAngle) - sweepWidth / 2.0;
    double endAngle = (360 - _sweepAngle) + sweepWidth / 2.0;

    QPainterPath beamPath;
    beamPath.moveTo(m_centerScreen);

    for (double angle = startAngle; angle <= endAngle; angle += 1.0) {
        double radians = qDegreesToRadians(angle);
        double x = m_centerScreen.x() + std::cos(radians) * m_maxRange * pixelPerMeterX;
        double y = m_centerScreen.y() - std::sin(radians) * m_maxRange * pixelPerMeterY;
        beamPath.lineTo(QPointF(x, y));
    }
    beamPath.closeSubpath();

    // --- Compute gradient direction along beam arc ---
    double startRad = qDegreesToRadians(startAngle);
    double endRad = qDegreesToRadians(endAngle);

    QPointF startEdge = QPointF(
        m_centerScreen.x() + std::cos(startRad) * m_maxRange * pixelPerMeterX,
        m_centerScreen.y() - std::sin(startRad) * m_maxRange * pixelPerMeterY
    );
    QPointF endEdge = QPointF(
        m_centerScreen.x() + std::cos(endRad) * m_maxRange * pixelPerMeterX,
        m_centerScreen.y() - std::sin(endRad) * m_maxRange * pixelPerMeterY
    );

    // --- Linear gradient from left to right edge ---
    QLinearGradient gradient(startEdge, endEdge);
    gradient.setColorAt(0.0, QColor(0, 255, 255, 180));  // Dark Blue
    gradient.setColorAt(0.8, QColor(0, 255, 255, 0));    // Transparent

    painter->setBrush(gradient);
    painter->setPen(Qt::NoPen);
    painter->drawPath(beamPath);
}
