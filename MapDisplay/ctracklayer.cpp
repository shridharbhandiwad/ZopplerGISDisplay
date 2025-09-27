#include "ctracklayer.h"
#include <qgsmaptopixel.h>
#include <QtMath>
#include <QPainter>
#include  "../cdatawarehouse.h"
#include "globalmacros.h"

int nAnimFrame = 0;

/**
 * @brief CTrackLayer constructor
 * @param pCanvas Pointer to the QgsMapCanvas
 */

CTrackLayer::CTrackLayer(QgsMapCanvas *canvas)
    : QgsMapCanvasItem(canvas), m_canvas(canvas)
{
    setZValue(101); // Ensure drawing order: above base map, below UI overlays
    QObject::connect(&m_timer, &QTimer::timeout, this, &CTrackLayer::_UpdateAnimation);
    m_timer.start(150); // ~10 FPS for smooth animation
}

/**
 * @brief Returns bounding rectangle for the layer — covers the whole canvas
 * @return QRectF bounding rectangle
 */
QRectF CTrackLayer::boundingRect() const
{
    return m_canvas->rect();
}

void CTrackLayer::_UpdateAnimation()
{
    nAnimFrame = (nAnimFrame + 2) % 20; // Loop from 0–19
    update(); // Redraw
}

/**
 * @brief Paints the tracks on the canvas
 * @param pPainter QPainter instance used for drawing
 */
void CTrackLayer::paint(QPainter *pPainter)
{
    if (!pPainter) return;

    pPainter->setRenderHint(QPainter::Antialiasing, true);

    // Transform geographic positions to screen coordinates
    const QgsMapToPixel &mapToPixel = m_canvas->mapSettings().mapToPixel();

    QList<stTrackDisplayInfo> listTracks = CDataWarehouse::getInstance()->getTrackList();

    for (const stTrackDisplayInfo &track : listTracks) {
        QPointF ptScreen = mapToPixel.transform(QgsPointXY(track.lon, track.lat)).toQPointF();
        double pixelPerDegree = 1.0 / m_canvas->mapUnitsPerPixel();
        QColor  clr = Qt::cyan;
        switch (track.nTrackIden)  {
        case TRACK_IDENTITY_UNKNOWN : clr = Qt::yellow;
                break;
        case TRACK_IDENTITY_FRIEND :  clr  = Qt::green;
           break;
        case TRACK_IDENTITY_HOSTILE : clr =  Qt::red;;
           break;

        }
        if ( pixelPerDegree > PPI_VISIBLE_THRESHOLD ) {
            // Core dot
            pPainter->setPen(clr);
            pPainter->setBrush(clr);
            pPainter->drawEllipse(ptScreen, 4, 4);

            // Heading line
            double dHeadingRad = qDegreesToRadians(track.heading);
            QPointF ptTip(
                ptScreen.x() + std::cos(dHeadingRad) * 10,
                ptScreen.y() - std::sin(dHeadingRad) * 10
            );
            pPainter->drawLine(ptScreen, ptTip);

            if ( pixelPerDegree > TEXT_VISIBLE_THRESHOLD ) {
                pPainter->setFont(QFont("century",11,80,true));
                // Label
                pPainter->setPen(Qt::white);
                pPainter->drawText(ptScreen + QPointF(6, -6), QString::number(track.nTrkId));
            }
        }

        // Glowing blip animation
        int nMaxRadius = 20;
        int nCurrentRadius = 4 + (nAnimFrame * (nMaxRadius - 4) / 20);
        int nAlpha = 255 - (nAnimFrame * 255 / 20); // Fades out

        // Gradient blip: transparent center → bright edge
        QRadialGradient gradient(ptScreen, nCurrentRadius);
        QColor clrEdge(clr.red(), clr.green(), clr.blue(), nAlpha);    // Bright neon green at edge
        //QColor clrCenter(0, 255, 255, 0);       // Fully transparent center

        gradient.setColorAt(0, Qt::cyan);
        gradient.setColorAt(0.5, clr);   // Center of the blip
        gradient.setColorAt(1.0, clrEdge);     // Edge of the blip

        pPainter->setPen(Qt::NoPen);
        pPainter->setBrush(gradient);
        pPainter->drawEllipse(ptScreen, nCurrentRadius, nCurrentRadius);
    }
}
