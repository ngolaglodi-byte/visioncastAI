/// @file monitoring_panel.cpp
/// @brief MonitoringPanel implementation.

#include "visioncast_ui/monitoring_panel.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>

namespace visioncast_ui {

MonitoringPanel::MonitoringPanel(QWidget* parent)
    : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);

    tabWidget_ = new QTabWidget(this);

    tabWidget_->addTab(createMetricsTab(), "System Metrics");
    tabWidget_->addTab(createLogTab(aiLogView_), "AI Logs");
    tabWidget_->addTab(createLogTab(engineLogView_), "Engine Logs");
    tabWidget_->addTab(createLogTab(zmqLogView_), "ZeroMQ Logs");

    mainLayout->addWidget(tabWidget_);

    refreshTimer_ = new QTimer(this);
    refreshTimer_->setInterval(1000); // Refresh every second

    setLayout(mainLayout);
}

QWidget* MonitoringPanel::createMetricsTab() {
    auto* widget = new QWidget(this);
    auto* layout = new QFormLayout(widget);

    cpuLabel_ = new QLabel("0%", widget);
    gpuLabel_ = new QLabel("0%", widget);
    latencyLabel_ = new QLabel("0 ms", widget);
    fpsLabel_ = new QLabel("0 fps", widget);
    droppedFramesLabel_ = new QLabel("0", widget);

    cpuBar_ = new QProgressBar(widget);
    cpuBar_->setRange(0, 100);
    gpuBar_ = new QProgressBar(widget);
    gpuBar_->setRange(0, 100);

    layout->addRow("CPU:", cpuBar_);
    layout->addRow("GPU:", gpuBar_);
    layout->addRow("Latency:", latencyLabel_);
    layout->addRow("FPS:", fpsLabel_);
    layout->addRow("Dropped Frames:", droppedFramesLabel_);

    widget->setLayout(layout);
    return widget;
}

QWidget* MonitoringPanel::createLogTab(QPlainTextEdit*& logView) {
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);

    logView = new QPlainTextEdit(widget);
    logView->setReadOnly(true);
    logView->setMaximumBlockCount(maxLogLines_);

    auto* clearButton = new QPushButton("Clear", widget);
    connect(clearButton, &QPushButton::clicked, logView, &QPlainTextEdit::clear);

    layout->addWidget(logView);
    layout->addWidget(clearButton);

    widget->setLayout(layout);
    return widget;
}

void MonitoringPanel::updateMetrics(const SystemMetrics& metrics) {
    cpuBar_->setValue(static_cast<int>(metrics.cpuUsage));
    gpuBar_->setValue(static_cast<int>(metrics.gpuUsage));
    latencyLabel_->setText(QString::number(metrics.latencyMs, 'f', 1) + " ms");
    fpsLabel_->setText(QString::number(metrics.fps, 'f', 1) + " fps");
    droppedFramesLabel_->setText(QString::number(metrics.droppedFrames));
}

void MonitoringPanel::appendLog(QPlainTextEdit* logView, const QString& source,
                                const QString& message, const QString& level) {
    if (!logView) return;
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString line = QString("[%1] [%2] [%3] %4").arg(timestamp, level, source, message);
    logView->appendPlainText(line);
}

void MonitoringPanel::appendAiLog(const QString& message, const QString& level) {
    appendLog(aiLogView_, "AI", message, level);
}

void MonitoringPanel::appendEngineLog(const QString& message, const QString& level) {
    appendLog(engineLogView_, "Engine", message, level);
}

void MonitoringPanel::appendZmqLog(const QString& message, const QString& level) {
    appendLog(zmqLogView_, "ZeroMQ", message, level);
}

void MonitoringPanel::clearAiLogs() {
    if (aiLogView_) aiLogView_->clear();
}

void MonitoringPanel::clearEngineLogs() {
    if (engineLogView_) engineLogView_->clear();
}

void MonitoringPanel::clearZmqLogs() {
    if (zmqLogView_) zmqLogView_->clear();
}

void MonitoringPanel::clearAllLogs() {
    clearAiLogs();
    clearEngineLogs();
    clearZmqLogs();
}

int MonitoringPanel::maxLogLines() const {
    return maxLogLines_;
}

void MonitoringPanel::setMaxLogLines(int maxLines) {
    maxLogLines_ = maxLines;
    if (aiLogView_) aiLogView_->setMaximumBlockCount(maxLines);
    if (engineLogView_) engineLogView_->setMaximumBlockCount(maxLines);
    if (zmqLogView_) zmqLogView_->setMaximumBlockCount(maxLines);
}

} // namespace visioncast_ui
