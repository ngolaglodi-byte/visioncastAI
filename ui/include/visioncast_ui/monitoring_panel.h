#pragma once

/// @file monitoring_panel.h
/// @brief System health monitoring panel with log views.

#include <QWidget>
#include <QString>
#include <QDateTime>

class QLabel;
class QProgressBar;
class QTimer;
class QTabWidget;
class QPlainTextEdit;

namespace visioncast_ui {

/// System performance metrics.
struct SystemMetrics {
    double cpuUsage = 0.0;
    double gpuUsage = 0.0;
    double latencyMs = 0.0;
    double fps = 0.0;
    int droppedFrames = 0;
};

/// A single log entry for the monitoring panel.
struct LogEntry {
    QDateTime timestamp;
    QString source;
    QString level;
    QString message;
};

/// Displays system health metrics and log views for AI, Engine, and ZeroMQ.
class MonitoringPanel : public QWidget {
    Q_OBJECT

public:
    explicit MonitoringPanel(QWidget* parent = nullptr);

    void updateMetrics(const SystemMetrics& metrics);

    void appendAiLog(const QString& message, const QString& level = "INFO");
    void appendEngineLog(const QString& message, const QString& level = "INFO");
    void appendZmqLog(const QString& message, const QString& level = "INFO");

    void clearAiLogs();
    void clearEngineLogs();
    void clearZmqLogs();
    void clearAllLogs();

    int maxLogLines() const;
    void setMaxLogLines(int maxLines);

private:
    QWidget* createMetricsTab();
    QWidget* createLogTab(QPlainTextEdit*& logView);

    void appendLog(QPlainTextEdit* logView, const QString& source,
                   const QString& message, const QString& level);

    // Metrics widgets
    QLabel* cpuLabel_ = nullptr;
    QLabel* gpuLabel_ = nullptr;
    QLabel* latencyLabel_ = nullptr;
    QLabel* fpsLabel_ = nullptr;
    QLabel* droppedFramesLabel_ = nullptr;
    QProgressBar* cpuBar_ = nullptr;
    QProgressBar* gpuBar_ = nullptr;
    QTimer* refreshTimer_ = nullptr;

    // Tab widget
    QTabWidget* tabWidget_ = nullptr;

    // Log views
    QPlainTextEdit* aiLogView_ = nullptr;
    QPlainTextEdit* engineLogView_ = nullptr;
    QPlainTextEdit* zmqLogView_ = nullptr;

    int maxLogLines_ = 1000;
};

} // namespace visioncast_ui
