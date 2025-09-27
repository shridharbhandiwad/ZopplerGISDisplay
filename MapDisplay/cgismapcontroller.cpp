#include "cgismapcontroller.h"


CGISMapController::CGISMapController(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsInputMethod, true);
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptTouchEvents(true);

    //displ = new CMapDisplay();
    //displ->show();;
    _m_pObjMapCanvas = new CMapCanvas();
    //_m_pObjMapCanvas->setMouseTracking(true);
    _m_pObjMapCanvas->Initialize();
    _m_pObjMapCanvas->refresh();
    _m_pObjMapCanvas->show();
    _m_pObjMapCanvas->

    connect( this, &QQuickPaintedItem::widthChanged, this, &CGISMapController::updateMapCanvasSize );
    connect( this, &QQuickPaintedItem::heightChanged, this, &CGISMapController::updateMapCanvasSize );

    connect(&_animationTimer, &QTimer::timeout, this, [this]() {
        //_frame++;
        update();  // Trigger paint()
    });
    _animationTimer.start(16); // ~60 FPS

}


CGISMapController::~CGISMapController()
{
    delete _m_pObjMapCanvas;
}

void CGISMapController::paint(QPainter *painter)
{
    if (!_m_pObjMapCanvas)
        return;


    //painter->setBrush(QBrush(Qt::red));
    //painter->drawRect(0,0,width(),height());

    //_m_pObjMapCanvas->fitInView(0,0,width(),height());
    //_m_pObjMapCanvas->render(painter);

  QImage image(_m_pObjMapCanvas->size(), QImage::Format_RGBA64_Premultiplied);
//    image.fill(Qt::red);

    QPainter imagePainter(&image);
    _m_pObjMapCanvas->render(&imagePainter);
    imagePainter.end();

    painter->drawImage(0, 0, image);
}

void CGISMapController::mousePressEvent( QMouseEvent* event )
{
    if (_m_pObjMapCanvas)  {
        _m_pObjMapCanvas->mousePressEvent(event);
    }

    QQuickPaintedItem::mousePressEvent(event);
}

void CGISMapController::mouseReleaseEvent( QMouseEvent* event )
{
    if (_m_pObjMapCanvas)  {
        _m_pObjMapCanvas->mouseReleaseEvent(event);
    }

    QQuickPaintedItem::mouseReleaseEvent(event);
}

void CGISMapController::hoverMoveEvent( QHoverEvent* event )
{
    if (_m_pObjMapCanvas)  {

        QPointF posF = event->pos();  // position in QQuickItem
        //QPoint pos = posF.toPoint();


        QMouseEvent mouseMoveEvent(
            QEvent::MouseMove,
            posF,
            Qt::NoButton,
            Qt::NoButton,
            event->modifiers()
        );
        _m_pObjMapCanvas->mouseMoveEvent(&mouseMoveEvent);
    }

    QQuickPaintedItem::hoverMoveEvent(event);
}

void CGISMapController::mouseDoubleClickEvent( QMouseEvent* event )
{
//    if (_m_pObjMapCanvas)  {
//        _m_pObjMapCanvas->mouseDoubleClickEvent(event);
//    }

    QQuickPaintedItem::mouseDoubleClickEvent(event);
}

void CGISMapController::wheelEvent( QWheelEvent *event )
{
    if (_m_pObjMapCanvas)  {
        _m_pObjMapCanvas->wheelEvent(event);
    }

    QQuickPaintedItem::wheelEvent(event);
}


void CGISMapController::timerEvent( QTimerEvent *event )
{

    QQuickPaintedItem::timerEvent(event);
}

void CGISMapController::routeMouseEvents( QMouseEvent* event )
{
    if (_m_pObjMapCanvas)
    {
        //QMouseEvent* newEvent = new QMouseEvent( event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers() );
        //QCoreApplication::postEvent( _m_pObjMapCanvas, newEvent );
        //update();
    }
}

void CGISMapController::routeWheelEvents( QWheelEvent* event )
{
    if (_m_pObjMapCanvas)
    {
        //QWheelEvent* newEvent = new QWheelEvent( event->pos(), event->delta(), event->buttons(), event->modifiers(), event->orientation() );
        //QCoreApplication::postEvent( _m_pObjMapCanvas, newEvent );
    }
}

void CGISMapController::updateMapCanvasSize()
{
    QPointF localPos = QPointF(0, 0);  // Top-left of the item
    QPointF scenePos = mapToScene(localPos);  // Converts to QQuickWindow scene coords

    QPoint globalPos;
    if (QQuickWindow *win = window()) {
        globalPos = win->mapToGlobal(scenePos.toPoint());  // Converts to screen coords
    }

    if (_m_pObjMapCanvas)
    {
        _m_pObjMapCanvas->setGeometry(globalPos.x(),globalPos.y(), (int)width(), (int)height());
        //update();
        //_m_pObjMapCanvas->update();;
        //_m_pObjMapCanvas->setViewport(QRect(0, 0, (int)width(), (int)height()));
    }
}

