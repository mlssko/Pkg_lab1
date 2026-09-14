#pragma once

#include <QObject>
#include <QColor>
#include <QString>

#include "../model/ColorMath.h"

class ColorViewModel : public QObject
{
    Q_OBJECT

public:
    explicit ColorViewModel(QObject* parent = nullptr);

    ColorMath::RGB rgb() const { return m_rgb; }
    ColorMath::CMYK cmyk() const { return m_cmyk; }
    ColorMath::HLS hls() const { return m_hls; }
    ColorMath::XYZ xyz() const { return m_xyz; }
    ColorMath::Lab lab() const { return m_lab; }

    ColorMath::Illuminant illuminant() const { return m_illuminant; }
    ColorMath::CmykAlgorithm cmykAlgorithm() const { return m_cmykAlgorithm; }
    ColorMath::GamutStrategy gamutStrategy() const { return m_gamutStrategy; }

    QColor toQColor() const;

public slots:
    void setRgb255(int r, int g, int b);
    void setCmykPercent(double c, double m, double y, double k);
    void setHls(double hDegrees, double lPercent, double sPercent);
    void setLab(double l, double a, double b);
    void setFromQColor(const QColor& color);

    void setIlluminant(ColorMath::Illuminant illuminant);
    void setCmykAlgorithm(ColorMath::CmykAlgorithm algorithm);
    void setGamutStrategy(ColorMath::GamutStrategy strategy);

signals:
    void colorChanged();
    void warningRaised(const QString& message);

private:
    void recomputeFromRgb();
    void finish(bool outOfGamut);

    ColorMath::RGB  m_rgb{1.0, 0.0, 0.0};
    ColorMath::CMYK m_cmyk;
    ColorMath::HLS  m_hls;
    ColorMath::XYZ  m_xyz;
    ColorMath::Lab  m_lab;

    ColorMath::Illuminant     m_illuminant     = ColorMath::Illuminant::D65;
    ColorMath::CmykAlgorithm  m_cmykAlgorithm   = ColorMath::CmykAlgorithm::GCR;
    ColorMath::GamutStrategy  m_gamutStrategy   = ColorMath::GamutStrategy::Clipping;
};