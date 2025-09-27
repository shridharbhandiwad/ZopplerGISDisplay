#ifndef CGISMAPCONTROLLER_H
#define CGISMAPCONTROLLER_H

#include <QObject>
#include "cmapcanvas.h"
#include <QWindow>
#include <QWidget>
#include <QQuickItem>
#include <QQuickWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QQuickPaintedItem>
#include "cmapdisplay.h"


class CGISMapController : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit CGISMapController(QQuickItem *parent = nullptr);
    ~CGISMapController();

    virtual void paint(QPainter *painter) override;
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void hoverMoveEvent(QHoverEvent  *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
private slots:
    void updateMapCanvasSize();
private:
    QPointer<CMapCanvas> _m_pObjMapCanvas;
    CMapDisplay *displ;

    QTimer _animationTimer;

    void routeMouseEvents(QMouseEvent *event);
    void routeWheelEvents(QWheelEvent *event);
};

#endif // CGISMAPCONTROLLER_H
