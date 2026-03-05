/// @file main_window.cpp
/// @brief MainWindow implementation.

#include "visioncast_ui/main_window.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include "visioncast_ui/monitoring_panel.h"
#include "visioncast_ui/output_config.h"
#include "visioncast_ui/overlay_editor.h"
#include "visioncast_ui/preview_panel.h"
#include "visioncast_ui/talent_manager.h"

namespace visioncast_ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupMenuBar();
    setupDockWidgets();
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open Configuration...");
    fileMenu->addAction("&Save Configuration...");
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close);

    auto* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Multiview");
    viewMenu->addAction("&Full Screen Preview");

    auto* broadcastMenu = menuBar()->addMenu("&Broadcast");
    broadcastMenu->addAction("&Go Live", this, &MainWindow::onGoLive);
    broadcastMenu->addAction("&Stop", this, &MainWindow::onStopBroadcast);
}

void MainWindow::setupDockWidgets() {
    // Central widget: Preview
    previewPanel_ = new PreviewPanel(this);
    setCentralWidget(previewPanel_);

    // Dock: Overlay Editor
    overlayEditor_ = new OverlayEditor(this);
    addDockWidget(Qt::RightDockWidgetArea, createDock("Overlay Editor", overlayEditor_));

    // Dock: Talent Manager
    talentManager_ = new TalentManager(this);
    addDockWidget(Qt::RightDockWidgetArea, createDock("Talent Database", talentManager_));

    // Dock: Output Config
    outputConfig_ = new OutputConfig(this);
    addDockWidget(Qt::BottomDockWidgetArea, createDock("Output Configuration", outputConfig_));

    // Dock: Monitoring
    monitoringPanel_ = new MonitoringPanel(this);
    addDockWidget(Qt::BottomDockWidgetArea, createDock("System Monitor", monitoringPanel_));
}

QDockWidget* MainWindow::createDock(const QString& title, QWidget* widget) {
    auto* dock = new QDockWidget(title, this);
    dock->setWidget(widget);
    return dock;
}

void MainWindow::onSourceChanged(const QString& source) {
    statusBar()->showMessage("Source: " + source);
}

void MainWindow::onOverlayToggled(bool enabled) {
    statusBar()->showMessage(enabled ? "Overlays ON" : "Overlays OFF");
}

void MainWindow::onGoLive() {
    statusBar()->showMessage("LIVE");
}

void MainWindow::onStopBroadcast() {
    statusBar()->showMessage("Broadcast stopped");
}

} // namespace visioncast_ui
