/// @file monitoring_panel.cpp
/// @brief MonitoringPanel implementation.

#include "visioncast_ui/monitoring_panel.h"

#include <QFormLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

namespace visioncast_ui {

MonitoringPanel::MonitoringPanel(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QFormLayout(this);

    cpuLabel_ = new QLabel("0%", this);
    gpuLabel_ = new QLabel("0%", this);
    latencyLabel_ = new QLabel("0 ms", this);
    fpsLabel_ = new QLabel("0 fps", this);
    droppedFramesLabel_ = new QLabel("0", this);

    cpuBar_ = new QProgressBar(this);
    cpuBar_->setRange(0, 100);
    gpuBar_ = new QProgressBar(this);
    gpuBar_->setRange(0, 100);

    layout->addRow("CPU:", cpuBar_);
    layout->addRow("GPU:", gpuBar_);
    layout->addRow("Latency:", latencyLabel_);
    layout->addRow("FPS:", fpsLabel_);
    layout->addRow("Dropped Frames:", droppedFramesLabel_);

    refreshTimer_ = new QTimer(this);
    refreshTimer_->setInterval(1000); // Refresh every second

    setLayout(layout);
}

void MonitoringPanel::updateMetrics(const SystemMetrics& metrics) {
    cpuBar_->setValue(static_cast<int>(metrics.cpuUsage));
    gpuBar_->setValue(static_cast<int>(metrics.gpuUsage));
    latencyLabel_->setText(QString::number(metrics.latencyMs, 'f', 1) + " ms");
    fpsLabel_->setText(QString::number(metrics.fps, 'f', 1) + " fps");
    droppedFramesLabel_->setText(QString::number(metrics.droppedFrames));
}

} // namespace visioncast_ui
