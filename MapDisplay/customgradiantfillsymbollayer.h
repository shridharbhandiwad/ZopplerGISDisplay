#ifndef CUSTOMGRADIANTFILLSYMBOLLAYER_H
#define CUSTOMGRADIANTFILLSYMBOLLAYER_H

#include <qgsfillsymbollayer.h>
#include <QLinearGradient>
#include <QObject>

class CustomGradiantFillSymbolLayer : public QgsSimpleFillSymbolLayer
{

public:
    CustomGradiantFillSymbolLayer();


    // Rendering method to override
    void render(QgsRenderContext &context, const QgsFeature &feature, QgsGeometry *geometry) const;

    QgsSimpleFillSymbolLayer* clone() const override;

    QString layerType() const override;

    // Optional serialization (commented out for now)
    // void readXml(const QDomNode &node) override;
    // void writeXml(QDomNode &node, QDomDocument &doc) const override;

    // Optional description
    // QString description() const override;
};

#endif // CUSTOMGRADIANTFILLSYMBOLLAYER_H
