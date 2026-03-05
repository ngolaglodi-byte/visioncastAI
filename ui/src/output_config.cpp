/// @file output_config.cpp
/// @brief OutputConfig implementation.

#include "visioncast_ui/output_config.h"

#include <QComboBox>
#include <QFormLayout>
#include <QPushButton>

namespace visioncast_ui {

OutputConfig::OutputConfig(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QFormLayout(this);

    deviceCombo_ = new QComboBox(this);
    deviceCombo_->addItems({"DeckLink", "AJA", "Magewell", "NDI", "Virtual"});

    resolutionCombo_ = new QComboBox(this);
    resolutionCombo_->addItems({"1920x1080 (1080p)", "3840x2160 (4K UHD)"});

    frameRateCombo_ = new QComboBox(this);
    frameRateCombo_->addItems({"23.98", "25", "29.97", "50", "59.94"});

    formatCombo_ = new QComboBox(this);
    formatCombo_->addItems({"SDI", "HDMI", "NDI", "SRT"});

    applyButton_ = new QPushButton("Apply", this);

    layout->addRow("Device:", deviceCombo_);
    layout->addRow("Resolution:", resolutionCombo_);
    layout->addRow("Frame Rate:", frameRateCombo_);
    layout->addRow("Output Format:", formatCombo_);
    layout->addRow(applyButton_);

    connect(applyButton_, &QPushButton::clicked, this, [this]() {
        emit settingsChanged(getCurrentSettings());
    });

    setLayout(layout);
}

void OutputConfig::refreshDevices() {
    // TODO: Enumerate available hardware devices from SDK
}

OutputSettings OutputConfig::getCurrentSettings() const {
    OutputSettings settings;
    settings.device = deviceCombo_->currentText();
    settings.resolution = resolutionCombo_->currentText();
    settings.frameRate = frameRateCombo_->currentText().toDouble();
    settings.format = formatCombo_->currentText();
    return settings;
}

} // namespace visioncast_ui
