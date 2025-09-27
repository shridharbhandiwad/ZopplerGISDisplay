#ifndef CPPILAYER_H
#define CPPILAYER_H

#include <qgsmapcanvas.h>
#include <qgsmapcanvasitem.h>
#include "csearchbeamlayer.h"

class CPPILayer : public QObject,public QgsMapCanvasItem
{
    Q_OBJECT
public:
    CPPILayer(QgsMapCanvas *canvas);

    void setCenter(const QgsPointXY &center);
    void setMaxRange(double rangeMeters);
    void setRangeRingCount(int count);
    void setAzimuthStep(int degrees);

    void paint(QPainter *painter) override;

    QgsRectangle boundingRectWorld() const;

protected:
    QRectF boundingRect() const override;


private:
    QgsMapCanvas *_m_canvas;
    CSearchBeamLayer *_m_searchbeamLayer;
    QgsPointXY m_center;
    double m_maxRange = 5000; // meters
    int m_ringCount = 5;
    int m_azimuthStep = 30; // degrees

    QTimer _animationTimer;
    double _sweepAngle = 0.0;

    void updatePosition() override;  // call this when center or canvas changes

    QPointF m_centerScreen;  // canvas pixel coordinates
    QgsMapCanvas *canvas();
};

#endif // CPPILAYER_H
