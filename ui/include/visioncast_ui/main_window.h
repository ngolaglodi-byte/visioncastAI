#pragma once

/// @file main_window.h
/// @brief Main application window with dockable panels.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include <QMainWindow>

class PreviewPanel;
class OverlayEditor;
class TalentManager;
class OutputConfig;
class MonitoringPanel;

namespace visioncast_ui {

class ControlRoom;
class DesignPanel;
class RecognitionPanel;
class LicenseManager;

/// Top-level application window for the VisionCast broadcast control room.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    /// Load and apply a Qt stylesheet (.qss) from the given file path.
    void loadTheme(const QString& qssPath);

private slots:
    void onSourceChanged(const QString& source);
    void onOverlayToggled(bool enabled);
    void onGoLive();
    void onStopBroadcast();
    void onEngineStartRequested(const QString& sourceName);
    void onEngineStopRequested();
    void onThemeSelected(const QString& themeName);
    void onManageLicense();
    void onAbout();
    void onLicenseBlocked(const QString& reason);

private:
    PreviewPanel* previewPanel_ = nullptr;
    OverlayEditor* overlayEditor_ = nullptr;
    TalentManager* talentManager_ = nullptr;
    OutputConfig* outputConfig_ = nullptr;
    MonitoringPanel* monitoringPanel_ = nullptr;
    ControlRoom* controlRoom_ = nullptr;
    DesignPanel* designPanel_ = nullptr;
    RecognitionPanel* recognitionPanel_ = nullptr;
    LicenseManager* licenseManager_ = nullptr;

    void setupMenuBar();
    void setupDockWidgets();
    QDockWidget* createDock(const QString& title, QWidget* widget);

    /// Shows a blocking license activation dialog on first run.
    /// @return true if the user successfully activated a license.
    bool showFirstRunLicenseActivation();

    /// Show a blocking error dialog and exit the application.
    void showLicenseBlockScreen(const QString& reason);
};

} // namespace visioncast_ui
