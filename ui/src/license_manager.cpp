/// @file license_manager.cpp
/// @brief LicenseManager implementation – communicates with the remote
///        Supabase Edge Function licensing API.

#include "visioncast_ui/license_manager.h"

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
                             "Please set api_url and api_key in "
                             "config/license.json."));
        return;
    }

    pendingAction_ = action;

    QNetworkRequest request(QUrl(apiUrl_));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/json"));
    request.setRawHeader("apikey", apiKey_.toUtf8());

    networkManager_->post(request,
                          QJsonDocument(payload).toJson(QJsonDocument::Compact));
}

void LicenseManager::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
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
            emit activationSucceeded(msg);
        } else {
            status_ = LicenseStatus::Invalid;
            emit activationFailed(msg);
        }
    } else if (pendingAction_ == "validate_key") {
        LicenseStatus st = mapStatus(obj.value("status").toString());
        const bool valid = success && st == LicenseStatus::Valid;
        status_ = valid ? LicenseStatus::Valid : st;
        emit validationCompleted(valid, msg);
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

} // namespace visioncast_ui
