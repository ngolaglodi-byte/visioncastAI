/// @file qml_bridge.cpp
/// @brief Implementation of QmlBridge — the C++ ↔ QML integration layer.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast_ui/qml_bridge.h"
#include "visioncast_ui/license_manager.h"
#include "visioncast_ui/license_config.h"
#include "visioncast_ui/python_launcher.h"
#include "visioncast_ui/device_scanner.h"

#include <QTimer>
#include <QVariantMap>
#include <QRandomGenerator>

namespace visioncast_ui {

// ── Construction / Destruction ─────────────────────────────────────────────

QmlBridge::QmlBridge(QObject* parent)
    : QObject(parent)
{
    initLicense();
    initPython();
    populateDemoData();

    // Metrics update every second
    metricsTimer_ = new QTimer(this);
    connect(metricsTimer_, &QTimer::timeout, this, &QmlBridge::onMetricsTimer);
    metricsTimer_->start(1000);
}

QmlBridge::~QmlBridge() = default;

// ── Private initialisation ─────────────────────────────────────────────────

void QmlBridge::initLicense()
{
    licenseManager_ = new LicenseManager(this);

    // Prefer env vars, fall back to config file
    if (!licenseManager_->loadFromEnvironment()) {
        licenseManager_->loadConfig("license_config.json");
    }

    connect(licenseManager_, &LicenseManager::validationCompleted,
            this, &QmlBridge::onLicenseValidation);

    // Try offline grace first; if ok mark as valid without a network round-trip
    if (licenseManager_->tryOfflineGrace()) {
        licenseValid_ = true;
        emit licenseStatusChanged();
    } else {
        const QString key = licenseManager_->licenseKey();
        if (!key.isEmpty()) {
            licenseManager_->validateKey(key);
        }
    }
}

void QmlBridge::initPython()
{
    pythonLauncher_ = new PythonLauncher(this);

    connect(pythonLauncher_, &PythonLauncher::aiStarted,
            this, &QmlBridge::onAiStarted);
    connect(pythonLauncher_, &PythonLauncher::aiStopped,
            this, &QmlBridge::onAiStopped);
    connect(pythonLauncher_, &PythonLauncher::error,
            this, [this](const QString& msg) {
                emit notification(msg, QStringLiteral("error"));
            });
}

void QmlBridge::populateDemoData()
{
    // Video sources — augmented by actual device scan when available
    auto devices = scanAllDevices();
    for (const auto& dev : devices) {
        QVariantMap src;
        src[QStringLiteral("name")]       = dev.name;
        src[QStringLiteral("deviceType")] = dev.deviceType;
        src[QStringLiteral("thumbnail")]  = QStringLiteral("");
        videoSources_.append(src);
    }
    emit sourcesChanged();

    // Demo talents
    const struct { const char* id; const char* name; const char* role; } talentData[] = {
        { "T001", "Alexandra Martin",  "Presenter"       },
        { "T002", "James Okoro",       "Co-Presenter"    },
        { "T003", "Dr. Sophie Dupont", "Expert Guest"    },
        { "T004", "Marc Bertrand",     "Sports Reporter" },
    };
    for (const auto& t : talentData) {
        QVariantMap m;
        m[QStringLiteral("id")]          = QString::fromLatin1(t.id);
        m[QStringLiteral("name")]        = QString::fromUtf8(t.name);
        m[QStringLiteral("role")]        = QString::fromLatin1(t.role);
        m[QStringLiteral("overlayActive")] = false;
        talents_.append(m);
    }
    emit talentsChanged();

    // Demo overlay templates
    const struct { const char* id; const char* name; const char* type; } overlayData[] = {
        { "OVL001", "Lower Third — Standard",  "lower_third" },
        { "OVL002", "Lower Third — Breaking",  "lower_third" },
        { "OVL003", "Ticker — News",           "ticker"      },
        { "OVL004", "Score Bug — Football",    "score_bug"   },
        { "OVL005", "Bug — Weather",           "bug"         },
        { "OVL006", "Full Screen — Transition","transition"  },
    };
    for (const auto& o : overlayData) {
        QVariantMap m;
        m[QStringLiteral("id")]     = QString::fromLatin1(o.id);
        m[QStringLiteral("name")]   = QString::fromLatin1(o.name);
        m[QStringLiteral("type")]   = QString::fromLatin1(o.type);
        m[QStringLiteral("active")] = false;
        overlayTemplates_.append(m);
    }
    emit overlaysChanged();

    // Output configurations
    const struct { const char* id; const char* name; const char* protocol; bool enabled; } outputData[] = {
        { "OUT001", "Primary SDI Out",  "SDI",  true  },
        { "OUT002", "NDI Stream",       "NDI",  true  },
        { "OUT003", "YouTube RTMP",     "RTMP", false },
        { "OUT004", "SRT Contribution", "SRT",  false },
    };
    for (const auto& o : outputData) {
        QVariantMap m;
        m[QStringLiteral("id")]       = QString::fromLatin1(o.id);
        m[QStringLiteral("name")]     = QString::fromLatin1(o.name);
        m[QStringLiteral("protocol")] = QString::fromLatin1(o.protocol);
        m[QStringLiteral("enabled")]  = o.enabled;
        outputConfigs_.append(m);
    }
    emit outputsChanged();
}

// ── Property accessors ─────────────────────────────────────────────────────

bool    QmlBridge::isEngineRunning() const { return engineRunning_; }
bool    QmlBridge::isAiConnected()   const { return aiConnected_;   }
int     QmlBridge::currentFps()      const { return fps_;           }
double  QmlBridge::cpuUsage()        const { return cpuUsage_;      }
double  QmlBridge::gpuUsage()        const { return gpuUsage_;      }
double  QmlBridge::memoryUsage()     const { return memoryUsage_;   }
QString QmlBridge::currentTalent()   const { return currentTalent_; }
bool    QmlBridge::isLive()          const { return isLive_;        }
bool    QmlBridge::isLicenseValid()  const { return licenseValid_;  }

QVariantList QmlBridge::videoSources()     const { return videoSources_;      }
QVariantList QmlBridge::talents()          const { return talents_;           }
QVariantList QmlBridge::overlayTemplates() const { return overlayTemplates_;  }
QVariantList QmlBridge::outputConfigs()    const { return outputConfigs_;     }

// ── Invokable methods ──────────────────────────────────────────────────────

void QmlBridge::selectSource(int index)
{
    if (index < 0 || index >= videoSources_.size()) return;
    selectedSourceIndex_ = index;
    const QVariantMap src = videoSources_.at(index).toMap();
    emit notification(QStringLiteral("Source selected: ") + src.value(QStringLiteral("name")).toString(),
                      QStringLiteral("info"));
}

void QmlBridge::startEngine()
{
    if (engineRunning_) return;
    engineRunning_ = true;
    fps_ = 25;
    emit engineStatusChanged();
    emit fpsChanged();
    emit notification(QStringLiteral("Broadcast engine started"), QStringLiteral("success"));
}

void QmlBridge::stopEngine()
{
    if (!engineRunning_) return;
    engineRunning_ = false;
    if (isLive_) stopBroadcast();
    fps_ = 0;
    emit engineStatusChanged();
    emit fpsChanged();
    emit notification(QStringLiteral("Broadcast engine stopped"), QStringLiteral("info"));
}

void QmlBridge::goLive()
{
    if (!engineRunning_) {
        emit notification(QStringLiteral("Start the engine first"), QStringLiteral("warning"));
        return;
    }
    isLive_ = true;
    emit liveStatusChanged();
    emit notification(QStringLiteral("🔴 ON AIR"), QStringLiteral("live"));
}

void QmlBridge::stopBroadcast()
{
    if (!isLive_) return;
    isLive_ = false;
    emit liveStatusChanged();
    emit notification(QStringLiteral("Broadcast stopped"), QStringLiteral("info"));
}

void QmlBridge::toggleOverlay(const QString& overlayId)
{
    for (int i = 0; i < overlayTemplates_.size(); ++i) {
        QVariantMap m = overlayTemplates_.at(i).toMap();
        if (m.value(QStringLiteral("id")).toString() == overlayId) {
            const bool newState = !m.value(QStringLiteral("active")).toBool();
            m[QStringLiteral("active")] = newState;
            overlayTemplates_[i] = m;
            emit overlaysChanged();
            emit notification(
                m.value(QStringLiteral("name")).toString() +
                    (newState ? QStringLiteral(" activated") : QStringLiteral(" deactivated")),
                QStringLiteral("info"));
            return;
        }
    }
}

void QmlBridge::selectTalent(const QString& talentId)
{
    for (const QVariant& v : talents_) {
        const QVariantMap m = v.toMap();
        if (m.value(QStringLiteral("id")).toString() == talentId) {
            currentTalent_ = m.value(QStringLiteral("name")).toString();
            emit talentChanged();
            emit notification(QStringLiteral("Talent: ") + currentTalent_, QStringLiteral("info"));
            return;
        }
    }
}

void QmlBridge::refreshSources()
{
    videoSources_.clear();
    populateDemoData();
    emit notification(QStringLiteral("Sources refreshed"), QStringLiteral("info"));
}

void QmlBridge::activateLicense(const QString& key)
{
    if (key.trimmed().isEmpty()) {
        emit notification(QStringLiteral("License key is empty"), QStringLiteral("error"));
        return;
    }
    licenseManager_->activateKey(key.trimmed());
}

QVariantMap QmlBridge::getSystemStatus()
{
    QVariantMap status;
    status[QStringLiteral("engineRunning")] = engineRunning_;
    status[QStringLiteral("aiConnected")]   = aiConnected_;
    status[QStringLiteral("isLive")]        = isLive_;
    status[QStringLiteral("fps")]           = fps_;
    status[QStringLiteral("cpuUsage")]      = cpuUsage_;
    status[QStringLiteral("gpuUsage")]      = gpuUsage_;
    status[QStringLiteral("memoryUsage")]   = memoryUsage_;
    status[QStringLiteral("licenseValid")]  = licenseValid_;
    return status;
}

// ── Private slots ──────────────────────────────────────────────────────────

void QmlBridge::onMetricsTimer()
{
    if (!engineRunning_) return;
    refreshMetrics();
}

void QmlBridge::refreshMetrics()
{
    // Simulate realistic metrics variance when no SDK monitoring is available
    auto rng = QRandomGenerator::global();
    cpuUsage_    = 20.0 + rng->bounded(30);
    gpuUsage_    = 35.0 + rng->bounded(25);
    memoryUsage_ = 40.0 + rng->bounded(20);
    fps_         = 24 + rng->bounded(3);
    emit metricsChanged();
    emit fpsChanged();
}

void QmlBridge::onAiStarted()
{
    aiConnected_ = true;
    emit aiStatusChanged();
    emit notification(QStringLiteral("AI recognition service connected"), QStringLiteral("success"));
}

void QmlBridge::onAiStopped()
{
    aiConnected_ = false;
    emit aiStatusChanged();
}

void QmlBridge::onLicenseValidation(bool valid, const QString& message)
{
    licenseValid_ = valid;
    emit licenseStatusChanged();
    if (!valid) {
        emit notification(message, QStringLiteral("error"));
    }
}

} // namespace visioncast_ui
