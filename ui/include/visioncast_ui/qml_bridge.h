#pragma once

/// @file qml_bridge.h
/// @brief C++ ↔ QML bridge that exposes VisionCast backend services to the
///        QML UI engine via Q_PROPERTY, Q_INVOKABLE, and signals.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QTimer>

namespace visioncast_ui {

class LicenseManager;
class PythonLauncher;

/// Bridge between the QML frontend and the C++ backend.
///
/// An instance of this class is registered as a QML context property
/// (name: "bridge") in main_qml.cpp.  All C++ backend logic stays in
/// existing classes; QmlBridge merely wires them together and converts
/// data to/from QVariant types that QML understands.
class QmlBridge : public QObject {
    Q_OBJECT

    // ── Runtime state ──────────────────────────────────────────────
    Q_PROPERTY(bool   engineRunning  READ isEngineRunning  NOTIFY engineStatusChanged)
    Q_PROPERTY(bool   aiConnected    READ isAiConnected    NOTIFY aiStatusChanged)
    Q_PROPERTY(int    fps            READ currentFps       NOTIFY fpsChanged)
    Q_PROPERTY(double cpuUsage       READ cpuUsage         NOTIFY metricsChanged)
    Q_PROPERTY(double gpuUsage       READ gpuUsage         NOTIFY metricsChanged)
    Q_PROPERTY(double memoryUsage    READ memoryUsage      NOTIFY metricsChanged)
    Q_PROPERTY(QString currentTalent READ currentTalent    NOTIFY talentChanged)
    Q_PROPERTY(bool   isLive         READ isLive           NOTIFY liveStatusChanged)
    Q_PROPERTY(bool   licenseValid   READ isLicenseValid   NOTIFY licenseStatusChanged)

    // ── Data lists ─────────────────────────────────────────────────
    Q_PROPERTY(QVariantList videoSources      READ videoSources      NOTIFY sourcesChanged)
    Q_PROPERTY(QVariantList talents           READ talents           NOTIFY talentsChanged)
    Q_PROPERTY(QVariantList overlayTemplates  READ overlayTemplates  NOTIFY overlaysChanged)
    Q_PROPERTY(QVariantList outputConfigs     READ outputConfigs     NOTIFY outputsChanged)

public:
    explicit QmlBridge(QObject* parent = nullptr);
    ~QmlBridge() override;

    // ── Property accessors ─────────────────────────────────────────
    bool    isEngineRunning() const;
    bool    isAiConnected()   const;
    int     currentFps()      const;
    double  cpuUsage()        const;
    double  gpuUsage()        const;
    double  memoryUsage()     const;
    QString currentTalent()   const;
    bool    isLive()          const;
    bool    isLicenseValid()  const;

    QVariantList videoSources()     const;
    QVariantList talents()          const;
    QVariantList overlayTemplates() const;
    QVariantList outputConfigs()    const;

    // ── Invokable methods (callable from QML) ──────────────────────

    /// Select a video source by index in the videoSources list.
    Q_INVOKABLE void selectSource(int index);

    /// Start the broadcast engine (begin processing video).
    Q_INVOKABLE void startEngine();

    /// Stop the broadcast engine.
    Q_INVOKABLE void stopEngine();

    /// Toggle the LIVE broadcast output on.
    Q_INVOKABLE void goLive();

    /// Stop the LIVE broadcast output.
    Q_INVOKABLE void stopBroadcast();

    /// Enable or disable an overlay by its unique ID.
    Q_INVOKABLE void toggleOverlay(const QString& overlayId);

    /// Set the active talent by ID.
    Q_INVOKABLE void selectTalent(const QString& talentId);

    /// Re-scan hardware devices and refresh videoSources.
    Q_INVOKABLE void refreshSources();

    /// Activate a license key.
    Q_INVOKABLE void activateLicense(const QString& key);

    /// Return a snapshot of the current system status as a map.
    Q_INVOKABLE QVariantMap getSystemStatus();

signals:
    void engineStatusChanged();
    void aiStatusChanged();
    void fpsChanged();
    void metricsChanged();
    void talentChanged();
    void liveStatusChanged();
    void licenseStatusChanged();
    void sourcesChanged();
    void talentsChanged();
    void overlaysChanged();
    void outputsChanged();

    /// Generic notification for the QML UI (e.g. toast messages).
    void notification(const QString& message, const QString& level);

private slots:
    void onMetricsTimer();
    void onAiStarted();
    void onAiStopped();
    void onLicenseValidation(bool valid, const QString& message);

private:
    void initLicense();
    void initPython();
    void populateDemoData();
    void refreshMetrics();

    LicenseManager* licenseManager_  = nullptr;
    PythonLauncher* pythonLauncher_   = nullptr;

    bool    engineRunning_ = false;
    bool    aiConnected_   = false;
    bool    isLive_        = false;
    bool    licenseValid_  = false;
    int     fps_           = 0;
    double  cpuUsage_      = 0.0;
    double  gpuUsage_      = 0.0;
    double  memoryUsage_   = 0.0;
    QString currentTalent_;
    int     selectedSourceIndex_ = -1;

    QVariantList videoSources_;
    QVariantList talents_;
    QVariantList overlayTemplates_;
    QVariantList outputConfigs_;

    QTimer* metricsTimer_ = nullptr;
};

} // namespace visioncast_ui
