/// @file main_window.cpp
/// @brief MainWindow implementation.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast_ui/main_window.h"

#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>
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
#include "visioncast_ui/license_config.h"
#include "visioncast_ui/license_defaults.h"
#include "visioncast_ui/license_dialog.h"
#include "visioncast_ui/python_launcher.h"

namespace visioncast_ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , licenseManager_(new LicenseManager(this))
{
    // ── 1. Load API configuration (priority: env vars > config file > compiled defaults) ──
    if (!licenseManager_->loadFromEnvironment()) {
        if (!licenseManager_->loadConfig(QStringLiteral("config/license.json"))
            || licenseManager_->apiUrl().isEmpty()) {
            // Use compiled-in defaults — no user configuration needed.
            licenseManager_->setApiUrl(visioncast_ui::defaultApiUrl());
            licenseManager_->setApiKey(visioncast_ui::defaultApiKey());
        }
    }

    // Connect the blocking signal before any validation.
    connect(licenseManager_, &LicenseManager::licenseBlocked,
            this, &MainWindow::onLicenseBlocked);

    // ── 2. Check license before showing the main UI ──
    // Try to validate an existing cached license (from license.dat or config).
    bool hasValidLicense = false;

    if (!licenseManager_->licenseKey().isEmpty()) {
        // We have a cached key — try offline grace first (instant, no network).
        hasValidLicense = licenseManager_->tryOfflineGrace();
    }

    if (!hasValidLicense) {
        // No valid license found — show the activation dialog immediately.
        // This is MODAL and BLOCKING: the user MUST activate before proceeding.
        if (!showFirstRunLicenseActivation()) {
            // User closed the dialog without activating — quit.
            QTimer::singleShot(0, qApp, &QApplication::quit);
            return;
        }
    }

    // ── 3. License is valid — set up the full UI ──
    setupMenuBar();
    setupDockWidgets();
    statusBar()->showMessage("Ready — License active");

    // Silently validate cached license key on startup.
    if (!licenseManager_->licenseKey().isEmpty())
        licenseManager_->validateKey(licenseManager_->licenseKey());

    // ── Python AI auto-launch ──
    pythonLauncher_ = new PythonLauncher(this);
    connect(pythonLauncher_, &PythonLauncher::aiStarted,
            this, &MainWindow::onAiStarted);
    connect(pythonLauncher_, &PythonLauncher::aiStopped,
            this, &MainWindow::onAiStopped);
    connect(pythonLauncher_, &PythonLauncher::error,
            this, &MainWindow::onAiError);
    connect(pythonLauncher_, &PythonLauncher::logOutput,
            this, &MainWindow::onAiLog);
    connect(pythonLauncher_, &PythonLauncher::setupProgress,
            this, [this](const QString& msg) {
                statusBar()->showMessage(msg);
            });

    // Auto-start the AI service.
    pythonLauncher_->start();
}

MainWindow::~MainWindow() {
    if (pythonLauncher_)
        pythonLauncher_->stop();
}

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
    helpMenu->addAction("À &propos…", this, &MainWindow::onAbout);
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
    if (pythonLauncher_ && !pythonLauncher_->isRunning())
        pythonLauncher_->start();
    statusBar()->showMessage("LIVE");
}

void MainWindow::onStopBroadcast() {
    if (pythonLauncher_)
        pythonLauncher_->stop();
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

void MainWindow::onAiStarted() {
    statusBar()->showMessage("AI recognition service started");
}

void MainWindow::onAiStopped() {
    statusBar()->showMessage("AI recognition service stopped");
}

void MainWindow::onAiError(const QString& error) {
    statusBar()->showMessage("AI error: " + error);
    qWarning() << "[PythonLauncher]" << error;
}

void MainWindow::onAiLog(const QString& line) {
    // Forward to the monitoring panel if available.
    qDebug() << "[AI]" << line;
}

void MainWindow::onAbout() {
    QMessageBox::about(this, tr("À propos de VisionCast-AI"),
        tr("VisionCast-AI — Licence officielle Prestige Technologie "
           "Company, développée par Glody Dimputu Ngola.\n\n"
           "Système de production broadcast alimenté par l'IA."));
}

void MainWindow::onLicenseBlocked(const QString& reason) {
    showLicenseBlockScreen(reason);
}

void MainWindow::showLicenseBlockScreen(const QString& reason) {
    QMessageBox::critical(this,
        tr("Licence invalide — VisionCast-AI ne peut pas démarrer."),
        reason);
    QApplication::quit();
}

} // namespace visioncast_ui
