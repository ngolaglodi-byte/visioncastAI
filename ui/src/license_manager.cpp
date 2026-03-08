/// @file license_manager.cpp
/// @brief LicenseManager implementation – communicates with the remote
///        licensing API and supports offline grace mode.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast_ui/license_manager.h"
#include "visioncast_ui/license_config.h"
#include "visioncast_ui/license_secure_logger.h"
#include "visioncast_ui/license_storage.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSysInfo>

namespace visioncast_ui {

// ── Construction ────────────────────────────────────────────────────

LicenseManager::LicenseManager(QObject* parent)
    : QObject(parent)
    , machineId_(generateMachineId())
    , networkManager_(new QNetworkAccessManager(this))
    , storage_(new LicenseStorage(machineId_))
{
    connect(networkManager_, &QNetworkAccessManager::finished,
            this, &LicenseManager::onReplyFinished);
}

// ── Configuration ───────────────────────────────────────────────────

bool LicenseManager::loadConfig(const QString& configPath) {
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();
    if (err.error != QJsonParseError::NoError)
        return false;

    const auto obj = doc.object();
    apiUrl_     = obj.value("api_url").toString();
    apiKey_     = obj.value("api_key").toString();
    licenseKey_ = obj.value("license_key").toString();
    return true;
}

bool LicenseManager::saveConfig(const QString& configPath) const {
    QJsonObject obj;
    obj["api_url"]     = apiUrl_;
    obj["api_key"]     = apiKey_;
    obj["license_key"] = licenseKey_;

    QFile file(configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool LicenseManager::loadFromEnvironment() {
    if (!LicenseConfig::load())
        return false;
    apiUrl_ = LicenseConfig::apiUrl();
    apiKey_ = LicenseConfig::apiKey();
    LicenseSecureLogger::logInfo(
        QStringLiteral("Configuration loaded from environment variables"));
    return true;
}

void LicenseManager::setApiUrl(const QString& url)  { apiUrl_ = url; }
QString LicenseManager::apiUrl() const               { return apiUrl_; }

void LicenseManager::setApiKey(const QString& key)   { apiKey_ = key; }

// ── Machine ID ──────────────────────────────────────────────────────

QString LicenseManager::machineId() const { return machineId_; }

QString LicenseManager::generateMachineId() {
    // Build a stable fingerprint from machine-unique attributes.
    QString raw = QSysInfo::machineUniqueId();
    if (raw.isEmpty()) {
        // Fallback: combine hostname + kernel + CPU architecture.
        raw = QSysInfo::machineHostName()
            + QSysInfo::kernelType()
            + QSysInfo::currentCpuArchitecture();
    }
    return QString::fromLatin1(
        QCryptographicHash::hash(raw.toUtf8(), QCryptographicHash::Sha256)
            .toHex());
}

// ── License Operations ──────────────────────────────────────────────

void LicenseManager::activateKey(const QString& licenseKey) {
    QJsonObject payload;
    payload["action"]     = QStringLiteral("activate_key");
    payload["key"]        = licenseKey;
    payload["machine_id"] = machineId_;
    sendRequest(payload, QStringLiteral("activate_key"));
}

void LicenseManager::validateKey(const QString& licenseKey) {
    QJsonObject payload;
    payload["action"]     = QStringLiteral("validate_key");
    payload["key"]        = licenseKey;
    payload["machine_id"] = machineId_;
    sendRequest(payload, QStringLiteral("validate_key"));
}

void LicenseManager::deactivateKey(const QString& licenseKey) {
    QJsonObject payload;
    payload["action"]     = QStringLiteral("deactivate_key");
    payload["key"]        = licenseKey;
    payload["machine_id"] = machineId_;
    sendRequest(payload, QStringLiteral("deactivate_key"));
}

void LicenseManager::checkStatus(const QString& licenseKey) {
    QJsonObject payload;
    payload["action"]     = QStringLiteral("check_status");
    payload["key"]        = licenseKey;
    payload["machine_id"] = machineId_;
    sendRequest(payload, QStringLiteral("check_status"));
}

// ── Offline Grace Mode ──────────────────────────────────────────────

bool LicenseManager::tryOfflineGrace() {
    if (!storage_->load(QLatin1String(kLicenseDatPath)))
        return false;

    if (storage_->isTampered()) {
        LicenseSecureLogger::logError(
            QStringLiteral("license.dat tampered — offline grace denied"));
        status_ = LicenseStatus::Invalid;
        return false;
    }

    licenseKey_        = storage_->licenseKey();
    offlineValidUntil_ = storage_->offlineValidUntil();

    if (QDateTime::currentDateTimeUtc() <= offlineValidUntil_) {
        status_ = LicenseStatus::Valid;
        LicenseSecureLogger::logOfflineModeEnabled();
        emit offlineModeActivated();
        return true;
    }

    LicenseSecureLogger::logError(
        QStringLiteral("offline grace period expired"));
    status_ = LicenseStatus::Expired;
    return false;
}

QDateTime LicenseManager::offlineValidUntil() const {
    return offlineValidUntil_;
}

// ── Blocking Check ──────────────────────────────────────────────────

bool LicenseManager::shouldBlockApplication() const {
    return status_ == LicenseStatus::Invalid
        || status_ == LicenseStatus::Expired
        || status_ == LicenseStatus::Suspended;
}

QString LicenseManager::blockReason() const {
    switch (status_) {
    case LicenseStatus::Invalid:
        return tr("Licence invalide — VisionCast-AI ne peut pas démarrer.");
    case LicenseStatus::Expired:
        return tr("Licence expirée — VisionCast-AI ne peut pas démarrer.");
    case LicenseStatus::Suspended:
        return tr("Licence suspendue — VisionCast-AI ne peut pas démarrer.");
    default:
        return {};
    }
}

// ── Current State ───────────────────────────────────────────────────

LicenseManager::LicenseStatus LicenseManager::status() const {
    return status_;
}

QString LicenseManager::licenseKey() const { return licenseKey_; }

bool LicenseManager::isLicensed() const {
    return status_ == LicenseStatus::Valid;
}

// ── Network ─────────────────────────────────────────────────────────

void LicenseManager::sendRequest(const QJsonObject& payload,
                                 const QString& action) {
    if (apiUrl_.isEmpty() || apiKey_.isEmpty()) {
        emit networkError(tr("License API is not configured. "
                             "Please set LICENSE_API_URL and LICENSE_API_KEY "
                             "environment variables."));
        return;
    }

    pendingAction_ = action;

    QNetworkRequest request{QUrl(apiUrl_)};
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/json"));
    request.setRawHeader("apikey", apiKey_.toUtf8());

    networkManager_->post(request,
                          QJsonDocument(payload).toJson(QJsonDocument::Compact));
}

void LicenseManager::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        LicenseSecureLogger::logError(reply->errorString());
        emit networkError(reply->errorString());
        return;
    }

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(reply->readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        emit networkError(tr("Invalid JSON response from license server."));
        return;
    }

    const auto obj     = doc.object();
    const bool success = obj.value("success").toBool(false);
    const QString msg  = obj.value("message").toString();

    // Map string status to enum.
    auto mapStatus = [](const QString& s) -> LicenseStatus {
        if (s == "valid"    || s == "active")    return LicenseStatus::Valid;
        if (s == "expired")                      return LicenseStatus::Expired;
        if (s == "suspended")                    return LicenseStatus::Suspended;
        return LicenseStatus::Invalid;
    };

    if (pendingAction_ == "activate_key") {
        if (success) {
            licenseKey_ = obj.value("key").toString(licenseKey_);
            status_ = LicenseStatus::Valid;
            refreshOfflineDeadline();
            LicenseSecureLogger::logValidation(QStringLiteral("active"));
            emit activationSucceeded(msg);
        } else {
            status_ = LicenseStatus::Invalid;
            LicenseSecureLogger::logError(msg);
            emit activationFailed(msg);
        }
    } else if (pendingAction_ == "validate_key") {
        LicenseStatus st = mapStatus(obj.value("status").toString());
        const bool valid = success && st == LicenseStatus::Valid;
        status_ = valid ? LicenseStatus::Valid : st;
        LicenseSecureLogger::logValidation(
            obj.value("status").toString(QStringLiteral("unknown")));
        if (valid)
            refreshOfflineDeadline();
        emit validationCompleted(valid, msg);

        if (shouldBlockApplication()) {
            LicenseSecureLogger::logError(blockReason());
            emit licenseBlocked(blockReason());
        }
    } else if (pendingAction_ == "deactivate_key") {
        if (success) {
            status_ = LicenseStatus::Unknown;
            licenseKey_.clear();
            emit deactivationSucceeded(msg);
        } else {
            emit deactivationFailed(msg);
        }
    } else if (pendingAction_ == "check_status") {
        LicenseStatus st = success
            ? mapStatus(obj.value("status").toString())
            : LicenseStatus::Invalid;
        status_ = st;
        emit statusChecked(st, msg);
    }

    pendingAction_.clear();
}

// ── Private Helpers ─────────────────────────────────────────────────

void LicenseManager::refreshOfflineDeadline() {
    offlineValidUntil_ = QDateTime::currentDateTimeUtc().addDays(
        kOfflineGraceDays);
    persistLicenseDat();
}

void LicenseManager::persistLicenseDat() const {
    if (licenseKey_.isEmpty())
        return;
    if (!storage_->save(QLatin1String(kLicenseDatPath),
                        licenseKey_, offlineValidUntil_)) {
        LicenseSecureLogger::logError(
            QStringLiteral("Failed to persist license.dat"));
    }
}

} // namespace visioncast_ui
