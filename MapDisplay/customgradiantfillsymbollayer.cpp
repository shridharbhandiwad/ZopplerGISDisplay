#include "customgradiantfillsymbollayer.h"
#include <QPainter>
#include <qgsrendercontext.h>
#include <qgsfeature.h>
#include <qgsgeometry.h>

CustomGradiantFillSymbolLayer::CustomGradiantFillSymbolLayer()
    : QgsSimpleFillSymbolLayer()
{
}

QgsSimpleFillSymbolLayer *CustomGradiantFillSymbolLayer::clone() const
{
    // Just create a new instance, don't copy
    return new CustomGradiantFillSymbolLayer();
}

QString CustomGradiantFillSymbolLayer::layerType() const
{
    return QStringLiteral("CustomGradientFill");
}

void CustomGradiantFillSymbolLayer::render(QgsRenderContext &context, const QgsFeature &feature, QgsGeometry *geometry) const
{
    if (!geometry)
        return;

    QPainter *painter = context.painter();
    if (!painter)
        return;

    QPolygonF poly = geometry->asQPolygonF();

    QPolygonF devPoly;
    for (const QPointF &pt : poly)
    {
        QPointF devPt(pt);
        context.mapToPixel().transform(devPt);  // Note: transform 'devPt' not 'pt'
        devPoly << devPt;
    }

    QLinearGradient gradient(devPoly.boundingRect().topLeft(), devPoly.boundingRect().topRight());

//    gradient.setColorAt(0, QColor("#3399FF"));
//    gradient.setColorAt(1, QColor("#001122"));

    gradient.setColorAt(0, QColor(Qt::red));
    gradient.setColorAt(1, QColor(Qt::green));

    QBrush brush(gradient);
    brush.setColor(Qt::white);

    painter->setBrush(brush);
   // painter->setPen(QPen(QColor("#3366FF"), 0.8));
    painter->drawPolygon(devPoly);
}
