#pragma once

#include <QObject>
#include <QColor>
#include <QString>

#include "../model/ColorMath.h"

// ColorViewModel - "ViewModel/Controller" слой
//
// Хранит единственный источник истины - текущий цвет в нормированном RGB -
// и текущие настройки (алгоритм цветоделения CMYK, стратегию обработки
// выхода за пределы модели). При любом изменении любого поля из любой
// модели пересчитывает представления во всех остальных моделях и извещает
// View сигналом colorChanged()
//

class ColorViewModel : public QObject
{
    Q_OBJECT

public:
    explicit ColorViewModel(QObject* parent = nullptr);

    ColorMath::RGB rgb() const { return m_rgb; }
    ColorMath::CMYK cmyk() const { return m_cmyk; }
    ColorMath::HLS hls() const { return m_hls; }

    ColorMath::CmykAlgorithm cmykAlgorithm() const { return m_cmykAlgorithm; }
    ColorMath::GamutStrategy gamutStrategy() const { return m_gamutStrategy; }

    // геттер для UI (0..255).
    QColor toQColor() const;

public slots:
    // Способ 1: точный ввод чисел (поля ввода) - способ 2: слайдеры -
    // способ 3: палитра (setFromQColor). Все три ведут в эти же слоты
    void setRgb255(int r, int g, int b);
    void setCmykPercent(double c, double m, double y, double k);
    void setHls(double hDegrees, double lPercent, double sPercent);
    void setFromQColor(const QColor& color);

    void setCmykAlgorithm(ColorMath::CmykAlgorithm algorithm);
    void setGamutStrategy(ColorMath::GamutStrategy strategy);

signals:
    // Испускается после каждого пересчёта - View должен перечитать все
    // геттеры и обновить виджеты
    void colorChanged();

    // Пустая строка = предупреждения нет (скрыть плашку в UI)
    void warningRaised(const QString& message);

private:
    void recomputeFromRgb();
    void finish(bool outOfGamut);

    ColorMath::RGB  m_rgb{1.0, 0.0, 0.0};
    ColorMath::CMYK m_cmyk;
    ColorMath::HLS  m_hls;

    ColorMath::CmykAlgorithm m_cmykAlgorithm = ColorMath::CmykAlgorithm::GCR;
    ColorMath::GamutStrategy m_gamutStrategy = ColorMath::GamutStrategy::Clipping;
};