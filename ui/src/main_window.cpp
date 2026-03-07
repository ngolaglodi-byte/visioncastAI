/// @file main_window.cpp
/// @brief MainWindow implementation.

#include "visioncast_ui/main_window.h"

#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include "visioncast_ui/monitoring_panel.h"
#include "visioncast_ui/output_config.h"
#include "visioncast_ui/overlay_editor.h"
#include "visioncast_ui/preview_panel.h"
#include "visioncast_ui/talent_manager.h"
#include "visioncast_ui/control_room.h"
#include "visioncast_ui/design_panel.h"
#include "visioncast_ui/recognition_panel.h"
#include "visioncast_ui/device_scanner.h"
#include "visioncast_ui/license_manager.h"
#include "visioncast_ui/license_dialog.h"

namespace visioncast_ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , licenseManager_(new LicenseManager(this))
{
    if (!licenseManager_->loadConfig(QStringLiteral("config/license.json")))
        qWarning("License config not loaded – check config/license.json");

    setupMenuBar();
    setupDockWidgets();
    statusBar()->showMessage("Ready");

    // Silently validate cached license key on startup.
    if (!licenseManager_->licenseKey().isEmpty())
        licenseManager_->validateKey(licenseManager_->licenseKey());
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
    viewMenu->addSeparator();

    auto* themeMenu = viewMenu->addMenu("&Theme");
    themeMenu->addAction("&Dark", this, [this]() { onThemeSelected("Dark"); });
    themeMenu->addAction("&Light", this, [this]() { onThemeSelected("Light"); });

    auto* broadcastMenu = menuBar()->addMenu("&Broadcast");
    broadcastMenu->addAction("&Go Live", this, &MainWindow::onGoLive);
    broadcastMenu->addAction("&Stop", this, &MainWindow::onStopBroadcast);

    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&Manage License…", this, &MainWindow::onManageLicense);
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

    // Dock: Control Room
    controlRoom_ = new ControlRoom(this);
    addDockWidget(Qt::LeftDockWidgetArea, createDock("Control Room", controlRoom_));

    // Dock: Design & Overlays
    designPanel_ = new DesignPanel(this);
    addDockWidget(Qt::RightDockWidgetArea, createDock("Design & Overlays", designPanel_));

    // Dock: AI Recognition
    recognitionPanel_ = new RecognitionPanel(this);
    addDockWidget(Qt::RightDockWidgetArea, createDock("AI Recognition", recognitionPanel_));

    connect(controlRoom_, &ControlRoom::sourceSelected,
            this, &MainWindow::onSourceChanged);
    connect(controlRoom_, &ControlRoom::engineStartRequested,
            this, &MainWindow::onEngineStartRequested);
    connect(controlRoom_, &ControlRoom::engineStopRequested,
            this, &MainWindow::onEngineStopRequested);

    // Populate the Control Room source list with real hardware devices.
    controlRoom_->setSources(scanAllDevices());
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

void MainWindow::onEngineStartRequested(const QString& sourceName) {
    statusBar()->showMessage("Engine started — " + sourceName);
}

void MainWindow::onEngineStopRequested() {
    statusBar()->showMessage("Engine stopped");
}

void MainWindow::loadTheme(const QString& qssPath) {
    QFile file(qssPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = file.readAll();
        file.close();
        qApp->setStyleSheet(styleSheet);
    } else {
        statusBar()->showMessage("Failed to load theme: " + qssPath);
    }
}

void MainWindow::onThemeSelected(const QString& themeName) {
    static const QString kThemeDark  = QStringLiteral("themes/dark.qss");
    static const QString kThemeLight = QStringLiteral("themes/light.qss");

    const QString& qssFile = (themeName == "Light") ? kThemeLight : kThemeDark;
    loadTheme(qssFile);
    statusBar()->showMessage("Theme: " + themeName);
}

void MainWindow::onManageLicense() {
    LicenseDialog dialog(licenseManager_, this);
    dialog.exec();

    // Persist any changes made via the dialog.
    if (!licenseManager_->saveConfig(QStringLiteral("config/license.json")))
        statusBar()->showMessage("Warning: could not save license config");
}

} // namespace visioncast_ui
