#pragma once

/// @file monitoring_panel.h
/// @brief System health monitoring panel.

#include <QWidget>

class QLabel;
class QProgressBar;
class QTimer;

namespace visioncast_ui {

/// System performance metrics.
struct SystemMetrics {
    double cpuUsage = 0.0;
    double gpuUsage = 0.0;
    double latencyMs = 0.0;
    double fps = 0.0;
    int droppedFrames = 0;
};

/// Displays system health metrics (CPU, GPU, latency, FPS, dropped frames).
class MonitoringPanel : public QWidget {
    Q_OBJECT

public:
    explicit MonitoringPanel(QWidget* parent = nullptr);

    void updateMetrics(const SystemMetrics& metrics);

private:
    QLabel* cpuLabel_ = nullptr;
    QLabel* gpuLabel_ = nullptr;
    QLabel* latencyLabel_ = nullptr;
    QLabel* fpsLabel_ = nullptr;
    QLabel* droppedFramesLabel_ = nullptr;
    QProgressBar* cpuBar_ = nullptr;
    QProgressBar* gpuBar_ = nullptr;
    QTimer* refreshTimer_ = nullptr;
};

} // namespace visioncast_ui
