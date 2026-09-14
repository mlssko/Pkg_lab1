#pragma once

#include <QMainWindow>

#include "../viewmodel/ColorViewModel.h"

class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QWidget* buildTopRow();
    QWidget* buildSettingsRow();
    QWidget* buildRgbGroup();
    QWidget* buildCmykGroup();
    QWidget* buildHlsGroup();
    QWidget* buildInfoGroup();

    void refreshUI();
    void updateGradients();

    ColorViewModel* m_viewModel;

    QLabel* m_swatch = nullptr;
    QPushButton* m_paletteButton = nullptr;
    QLineEdit* m_hexEdit = nullptr;

    QComboBox* m_illuminantCombo = nullptr;
    QComboBox* m_cmykAlgoCombo = nullptr;
    QComboBox* m_gamutStrategyCombo = nullptr;

    QSlider* m_sliderR = nullptr; QSpinBox* m_spinR = nullptr;
    QSlider* m_sliderG = nullptr; QSpinBox* m_spinG = nullptr;
    QSlider* m_sliderB = nullptr; QSpinBox* m_spinB = nullptr;

    QSlider* m_sliderC = nullptr; QDoubleSpinBox* m_spinC = nullptr;
    QSlider* m_sliderM = nullptr; QDoubleSpinBox* m_spinM = nullptr;
    QSlider* m_sliderY = nullptr; QDoubleSpinBox* m_spinY = nullptr;
    QSlider* m_sliderK = nullptr; QDoubleSpinBox* m_spinK = nullptr;

    QSlider* m_sliderH = nullptr; QDoubleSpinBox* m_spinH = nullptr;
    QSlider* m_sliderL = nullptr; QDoubleSpinBox* m_spinL = nullptr;
    QSlider* m_sliderS = nullptr; QDoubleSpinBox* m_spinS = nullptr;

    QLabel* m_xyzInfoLabel = nullptr;
    QDoubleSpinBox* m_spinLabL = nullptr;
    QDoubleSpinBox* m_spinLabA = nullptr;
    QDoubleSpinBox* m_spinLabB = nullptr;

    QLabel* m_warningLabel = nullptr;

    bool m_updatingUi = false;
};