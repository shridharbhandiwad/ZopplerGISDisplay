#include "cgismapcontroller.h"


CGISMapController::CGISMapController(QQuickItem *parent) : QQuickPaintedItem(parent), _needsRepaint(false)
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

    connect( this, &QQuickPaintedItem::widthChanged, this, &CGISMapController::updateMapCanvasSize );
    connect( this, &QQuickPaintedItem::heightChanged, this, &CGISMapController::updateMapCanvasSize );

    // Connect canvas refresh to our repaint flag
    connect(_m_pObjMapCanvas, &QgsMapCanvas::mapCanvasRefreshed, this, [this]() {
        _needsRepaint = true;
        update();
    });

    // Reduced update frequency - only update when needed
    connect(&_animationTimer, &QTimer::timeout, this, [this]() {
        // Only trigger repaint if something changed
        if (_needsRepaint) {
            update();
        }
    });
    _animationTimer.start(33); // ~30 FPS instead of 60 FPS - more efficient

}


CGISMapController::~CGISMapController()
{
    delete _m_pObjMapCanvas;
}

void CGISMapController::paint(QPainter *painter)
{
    if (!_m_pObjMapCanvas)
        return;

    // Only render if we need to repaint or cache is invalid
    if (_needsRepaint || _cachedImage.isNull() || _cachedImage.size() != _m_pObjMapCanvas->size()) {
        _cachedImage = QImage(_m_pObjMapCanvas->size(), QImage::Format_ARGB32_Premultiplied);
        
        QPainter imagePainter(&_cachedImage);
        _m_pObjMapCanvas->render(&imagePainter);
        imagePainter.end();
        
        _needsRepaint = false;
    }

    painter->drawImage(0, 0, _cachedImage);
}

void CGISMapController::mousePressEvent( QMouseEvent* event )
{
    if (_m_pObjMapCanvas)  {
        _m_pObjMapCanvas->mousePressEvent(event);
        _needsRepaint = true;  // Mark for repaint on mouse press
    }

    QQuickPaintedItem::mousePressEvent(event);
}

void CGISMapController::mouseReleaseEvent( QMouseEvent* event )
{
    if (_m_pObjMapCanvas)  {
        _m_pObjMapCanvas->mouseReleaseEvent(event);
        _needsRepaint = true;  // Mark for repaint on mouse release
    }

    QQuickPaintedItem::mouseReleaseEvent(event);
}

void CGISMapController::hoverMoveEvent( QHoverEvent* event )
{
    if (_m_pObjMapCanvas)  {
        // Throttle hover events - only process if enough time has passed
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - _lastHoverTime < 50) { // Throttle to max 20 updates/sec
            QQuickPaintedItem::hoverMoveEvent(event);
            return;
        }
        _lastHoverTime = currentTime;

        QPointF posF = event->pos();  // position in QQuickItem

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
        _needsRepaint = true;  // Mark for repaint on wheel event
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
        _needsRepaint = true;  // Mark for repaint on size change
        _cachedImage = QImage();  // Invalidate cache when size changes
    }
}

