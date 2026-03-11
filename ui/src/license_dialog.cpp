/// @file license_dialog.cpp
/// @brief Dialog for activating, validating, and managing a license key.

#include "visioncast_ui/license_dialog.h"

#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace visioncast_ui {

static QString configPathForSave()
{
    // Save next to the current working directory: <cwd>/config/license.json
    // This matches how MainWindow loads it: "config/license.json"
    return QDir::current().filePath(QStringLiteral("config/license.json"));
}

LicenseDialog::LicenseDialog(LicenseManager* manager, QWidget* parent)
    : QDialog(parent)
    , manager_(manager)
{
    setupUi();
    connectSignals();
    updateStatusDisplay();
}

void LicenseDialog::setupUi() {
    setWindowTitle(tr("License Management"));
    setModal(true);

    auto* root = new QVBoxLayout(this);

    auto* title = new QLabel(tr("<h2>Bienvenue dans VisionCast-AI</h2>"), this);
    title->setAlignment(Qt::AlignHCenter);
    root->addWidget(title);

    auto* subtitle = new QLabel(
        tr("Veuillez entrer votre clé de licence pour activer le logiciel."),
        this);
    subtitle->setAlignment(Qt::AlignHCenter);
    root->addWidget(subtitle);

    // Machine ID row
    auto* machineRow = new QHBoxLayout();
    machineRow->addWidget(new QLabel(tr("Machine ID:"), this));
    machineIdLabel_ = new QLabel(this);
    machineIdLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    machineRow->addWidget(machineIdLabel_, 1);
    root->addLayout(machineRow);

    // Key row
    auto* keyRow = new QHBoxLayout();
    keyRow->addWidget(new QLabel(tr("License Key:"), this));
    keyInput_ = new QLineEdit(this);
    keyInput_->setPlaceholderText(tr("XXXX-XXXX-XXXX-XXXX"));
    keyRow->addWidget(keyInput_, 1);
    root->addLayout(keyRow);

    // Status row
    auto* statusRow = new QHBoxLayout();
    statusRow->addWidget(new QLabel(tr("Status:"), this));
    statusLabel_ = new QLabel(tr("Not activated"), this);
    statusRow->addWidget(statusLabel_, 1);
    root->addLayout(statusRow);

    // Buttons
    auto* buttonsRow = new QHBoxLayout();
    activateButton_ = new QPushButton(tr("Activate"), this);
    deactivateButton_ = new QPushButton(tr("Deactivate"), this);
    checkButton_ = new QPushButton(tr("Check Status"), this);
    closeButton_ = new QPushButton(tr("Close"), this);

    buttonsRow->addWidget(activateButton_);
    buttonsRow->addWidget(deactivateButton_);
    buttonsRow->addWidget(checkButton_);
    buttonsRow->addStretch(1);
    buttonsRow->addWidget(closeButton_);
    root->addLayout(buttonsRow);

    // Init values
    if (manager_) {
        machineIdLabel_->setText(manager_->machineId());
        if (!manager_->licenseKey().isEmpty())
            keyInput_->setText(manager_->licenseKey());
    } else {
        machineIdLabel_->setText(tr("N/A"));
    }
}

void LicenseDialog::connectSignals() {
    // UI -> slots
    connect(activateButton_, &QPushButton::clicked, this, &LicenseDialog::onActivate);
    connect(deactivateButton_, &QPushButton::clicked, this, &LicenseDialog::onDeactivate);
    connect(checkButton_, &QPushButton::clicked, this, &LicenseDialog::onCheckStatus);
    connect(closeButton_, &QPushButton::clicked, this, &QDialog::reject);

    if (!manager_)
        return;

    // Manager -> slots
    connect(manager_, &LicenseManager::activationSucceeded,
            this, &LicenseDialog::onActivationSucceeded);
    connect(manager_, &LicenseManager::activationFailed,
            this, &LicenseDialog::onActivationFailed);
    connect(manager_, &LicenseManager::validationCompleted,
            this, &LicenseDialog::onValidationCompleted);
    connect(manager_, &LicenseManager::deactivationSucceeded,
            this, &LicenseDialog::onDeactivationSucceeded);
    connect(manager_, &LicenseManager::deactivationFailed,
            this, &LicenseDialog::onDeactivationFailed);
    connect(manager_, &LicenseManager::statusChecked,
            this, &LicenseDialog::onStatusChecked);
    connect(manager_, &LicenseManager::networkError,
            this, &LicenseDialog::onNetworkError);
}

void LicenseDialog::setUiBusy(bool busy) {
    if (activateButton_) activateButton_->setEnabled(!busy);
    if (deactivateButton_) deactivateButton_->setEnabled(!busy);
    if (checkButton_) checkButton_->setEnabled(!busy);
    if (closeButton_) closeButton_->setEnabled(!busy);
}

void LicenseDialog::updateStatusDisplay() {
    if (!manager_ || !statusLabel_)
        return;

    switch (manager_->status()) {
    case LicenseManager::LicenseStatus::Valid:
        statusLabel_->setText(tr("✓ Licensed"));
        break;
    case LicenseManager::LicenseStatus::Expired:
        statusLabel_->setText(tr("Expired"));
        break;
    case LicenseManager::LicenseStatus::Suspended:
        statusLabel_->setText(tr("Suspended"));
        break;
    case LicenseManager::LicenseStatus::Invalid:
        statusLabel_->setText(tr("Invalid"));
        break;
    default:
        statusLabel_->setText(tr("Not activated"));
        break;
    }
}

void LicenseDialog::onActivate() {
    if (!manager_)
        return;

    const QString key = keyInput_ ? keyInput_->text().trimmed() : QString();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("Activation"), tr("Please enter a license key."));
        return;
    }

    if (statusLabel_) statusLabel_->setText(tr("Contacting server..."));
    setUiBusy(true);
    manager_->activateKey(key);
}

void LicenseDialog::onDeactivate() {
    if (!manager_)
        return;

    const QString key = keyInput_ ? keyInput_->text().trimmed() : QString();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("Deactivation"), tr("Please enter a license key."));
        return;
    }

    if (statusLabel_) statusLabel_->setText(tr("Contacting server..."));
    setUiBusy(true);
    manager_->deactivateKey(key);
}

void LicenseDialog::onCheckStatus() {
    if (!manager_)
        return;

    const QString key = keyInput_ ? keyInput_->text().trimmed() : QString();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("Status"), tr("Please enter a license key."));
        return;
    }

    if (statusLabel_) statusLabel_->setText(tr("Contacting server..."));
    setUiBusy(true);
    manager_->checkStatus(key);
}

void LicenseDialog::onActivationSucceeded(const QString& message) {
    setUiBusy(false);

    // IMPORTANT: Some APIs don't echo back "key" in the JSON response.
    // Force persistence using the key the user typed.
    const QString typedKey = keyInput_ ? keyInput_->text().trimmed() : QString();
    if (!typedKey.isEmpty()) {
        // Requires setLicenseKey(...) in LicenseManager (add it if missing).
        manager_->setLicenseKey(typedKey);
    }

    const QString path = configPathForSave();
    const bool saved = manager_->saveConfig(path);

    updateStatusDisplay();

    if (!saved) {
        QMessageBox::warning(this, tr("Activation"),
                             tr("License activated, but could not save:\n%1\n\n"
                                "Check folder permissions and that the 'config' folder exists.")
                                 .arg(QFileInfo(path).absoluteFilePath()));
    }

    if (!message.isEmpty())
        QMessageBox::information(this, tr("Activation"), message);

    accept();
}

void LicenseDialog::onActivationFailed(const QString& error) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::critical(this, tr("Activation failed"),
                          error.isEmpty() ? tr("Activation failed.") : error);
}

void LicenseDialog::onValidationCompleted(bool valid, const QString& message) {
    setUiBusy(false);

    if (valid) {
        const QString typedKey = keyInput_ ? keyInput_->text().trimmed() : QString();
        if (!typedKey.isEmpty()) {
            manager_->setLicenseKey(typedKey);
        }

        manager_->saveConfig(configPathForSave());
    }

    updateStatusDisplay();

    if (!message.isEmpty())
        QMessageBox::information(this, tr("Validation"), message);

    if (valid) {
        accept();
    }
}

void LicenseDialog::onDeactivationSucceeded(const QString& message) {
    setUiBusy(false);

    // After deactivation, LicenseManager clears the key; persist that.
    manager_->saveConfig(configPathForSave());

    updateStatusDisplay();
    if (!message.isEmpty())
        QMessageBox::information(this, tr("Deactivation"), message);
}

void LicenseDialog::onDeactivationFailed(const QString& error) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::critical(this, tr("Deactivation failed"),
                          error.isEmpty() ? tr("Deactivation failed.") : error);
}

void LicenseDialog::onStatusChecked(LicenseManager::LicenseStatus status,
                                   const QString& message) {
    Q_UNUSED(status);

    setUiBusy(false);
    updateStatusDisplay();

    if (!message.isEmpty())
        QMessageBox::information(this, tr("Status"), message);

    if (manager_ && manager_->isLicensed()) {
        const QString typedKey = keyInput_ ? keyInput_->text().trimmed() : QString();
        if (!typedKey.isEmpty()) {
            manager_->setLicenseKey(typedKey);
        }
        manager_->saveConfig(configPathForSave());
        accept();
    }
}

void LicenseDialog::onNetworkError(const QString& error) {
    setUiBusy(false);
    if (statusLabel_) statusLabel_->setText(tr("Network error"));
    QMessageBox::critical(this, tr("Network Error"),
                          error.isEmpty() ? tr("Network error.") : error);
}

} // namespace visioncast_ui