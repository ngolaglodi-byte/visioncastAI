#pragma once

/// @file output_config.h
/// @brief Output device and encoder configuration panel.

#include <QWidget>

class QComboBox;
class QPushButton;

namespace visioncast_ui {

/// Output settings structure.
struct OutputSettings {
    QString device;
    QString resolution;
    double frameRate = 25.0;
    QString format;
};

/// Configuration panel for broadcast output devices and encoding settings.
class OutputConfig : public QWidget {
    Q_OBJECT

public:
    explicit OutputConfig(QWidget* parent = nullptr);

    void refreshDevices();
    OutputSettings getCurrentSettings() const;

signals:
    void settingsChanged(const OutputSettings& settings);

private:
    QComboBox* deviceCombo_ = nullptr;
    QComboBox* resolutionCombo_ = nullptr;
    QComboBox* frameRateCombo_ = nullptr;
    QComboBox* formatCombo_ = nullptr;
    QPushButton* applyButton_ = nullptr;
};

} // namespace visioncast_ui
