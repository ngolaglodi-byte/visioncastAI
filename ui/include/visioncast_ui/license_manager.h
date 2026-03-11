#pragma once

/// @file license_manager.h
/// @brief LicenseManager – communicates with the remote licensing API and
/// supports offline grace mode.

#include <QObject>
#include <QDateTime>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

namespace visioncast_ui {

class LicenseStorage;

class LicenseManager : public QObject {
    Q_OBJECT

public:
    enum class LicenseStatus {
        Unknown,
        Valid,
        Invalid,
        Expired,
        Suspended
    };
    Q_ENUM(LicenseStatus)

    explicit LicenseManager(QObject* parent = nullptr);

    // Config
    bool loadConfig(const QString& configPath);
    bool saveConfig(const QString& configPath) const;
    bool loadFromEnvironment();

    void setApiUrl(const QString& url);
    QString apiUrl() const;

    void setApiKey(const QString& key);

    // NEW: allows the UI to force persistence of the typed key
    void setLicenseKey(const QString& key);

    // Machine
    QString machineId() const;

    // Operations
    void activateKey(const QString& licenseKey);
    void validateKey(const QString& licenseKey);
    void deactivateKey(const QString& licenseKey);
    void checkStatus(const QString& licenseKey);

    // Offline grace
    bool tryOfflineGrace();
    QDateTime offlineValidUntil() const;

    // State
    LicenseStatus status() const;
    QString licenseKey() const;
    bool isLicensed() const;

    bool shouldBlockApplication() const;
    QString blockReason() const;

signals:
    void activationSucceeded(const QString& message);
    void activationFailed(const QString& error);

    void validationCompleted(bool valid, const QString& message);

    void deactivationSucceeded(const QString& message);
    void deactivationFailed(const QString& error);

    void statusChecked(LicenseManager::LicenseStatus status, const QString& message);

    void networkError(const QString& error);
    void licenseBlocked(const QString& reason);

    void offlineModeActivated();

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    static QString generateMachineId();

    void sendRequest(const QJsonObject& payload, const QString& action);

    void refreshOfflineDeadline();
    void persistLicenseDat() const;

private:
    QString machineId_;

    QString apiUrl_;
    QString apiKey_;
    QString licenseKey_;

    LicenseStatus status_ = LicenseStatus::Unknown;

    QString pendingAction_;

    QNetworkAccessManager* networkManager_ = nullptr;
    LicenseStorage* storage_ = nullptr;

    QDateTime offlineValidUntil_;

    // Constants (these likely exist in your license_config.h; keep here only if needed)
    static constexpr int kOfflineGraceDays = 7;
    static constexpr const char* kLicenseDatPath = "license.dat";
};

} // namespace visioncast_ui