#ifndef CTRACKLAYER_H
#define CTRACKLAYER_H

#include <qgsmapcanvasitem.h>
#include <qgsmapcanvas.h>
#include <QTimer>

class CTrackLayer : public QObject, public QgsMapCanvasItem
{
    Q_OBJECT
public:
    CTrackLayer(QgsMapCanvas *canvas);

    void paint(QPainter *painter) override;
    QRectF boundingRect() const override;

private slots:
    void _UpdateAnimation(); //!< Timer-based animation tick

private:
    QgsMapCanvas *m_canvas;
    QTimer m_timer;
};

#endif // CTRACKLAYER_H
