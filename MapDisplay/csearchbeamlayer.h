#ifndef CSEARCHBEAMLAYER_H
#define CSEARCHBEAMLAYER_H

#include <QObject>

#include <qgsmapcanvas.h>
#include <qgsmapcanvasitem.h>
#include <QTimer>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>

class CSearchBeamLayer : public QObject,public QgsMapCanvasItem
{
public:
    CSearchBeamLayer(QgsMapCanvas *canvas);

    void setCenter(const QgsPointXY &center);
    void setMaxRange(double rangeMeters);

    void paint(QPainter *painter) override;

protected:
    QRectF boundingRect() const override;

    QgsMapCanvas *_m_canvas;
    QgsPointXY m_center;
    double m_maxRange = 5000; // meters

    QTimer _animationTimer;
    double _sweepAngle = 0.0;

    void updatePosition() override;  // call this when center or canvas changes

    QPointF m_centerScreen;  // canvas pixel coordinates
    QgsMapCanvas *canvas();
};

#endif // CSEARCHBEAMLAYER_H
