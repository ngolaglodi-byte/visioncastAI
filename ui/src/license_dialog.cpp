/// @file license_dialog.cpp
/// @brief LicenseDialog implementation.

#include "visioncast_ui/license_dialog.h"
#include "visioncast_ui/license_manager.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace visioncast_ui {

LicenseDialog::LicenseDialog(LicenseManager* manager, QWidget* parent)
    : QDialog(parent)
    , manager_(manager)
{
    setWindowTitle(tr("License Management"));
    setMinimumWidth(460);
    setupUi();
    connectSignals();
    updateStatusDisplay();
}

// ── UI Setup ────────────────────────────────────────────────────────

void LicenseDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    // Machine ID (read-only)
    machineIdLabel_ = new QLabel(this);
    machineIdLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    {
        auto* row = new QHBoxLayout;
        row->addWidget(new QLabel(tr("Machine ID:"), this));
        row->addWidget(machineIdLabel_, 1);
        mainLayout->addLayout(row);
    }
    machineIdLabel_->setText(manager_->machineId());

    // License key input
    keyInput_ = new QLineEdit(this);
    keyInput_->setPlaceholderText(tr("XXXX-XXXX-XXXX-XXXX"));
    keyInput_->setText(manager_->licenseKey());
    {
        auto* row = new QHBoxLayout;
        row->addWidget(new QLabel(tr("License Key:"), this));
        row->addWidget(keyInput_, 1);
        mainLayout->addLayout(row);
    }

    // Status label
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet("font-weight: bold;");
    {
        auto* row = new QHBoxLayout;
        row->addWidget(new QLabel(tr("Status:"), this));
        row->addWidget(statusLabel_, 1);
        mainLayout->addLayout(row);
    }

    // Action buttons
    activateButton_   = new QPushButton(tr("Activate"), this);
    deactivateButton_ = new QPushButton(tr("Deactivate"), this);
    checkButton_      = new QPushButton(tr("Check Status"), this);
    closeButton_      = new QPushButton(tr("Close"), this);
    {
        auto* row = new QHBoxLayout;
        row->addWidget(activateButton_);
        row->addWidget(deactivateButton_);
        row->addWidget(checkButton_);
        row->addStretch();
        row->addWidget(closeButton_);
        mainLayout->addLayout(row);
    }
}

void LicenseDialog::connectSignals() {
    connect(activateButton_,   &QPushButton::clicked,
            this, &LicenseDialog::onActivate);
    connect(deactivateButton_, &QPushButton::clicked,
            this, &LicenseDialog::onDeactivate);
    connect(checkButton_,      &QPushButton::clicked,
            this, &LicenseDialog::onCheckStatus);
    connect(closeButton_,      &QPushButton::clicked,
            this, &QDialog::accept);

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

// ── Slot Implementations ────────────────────────────────────────────

void LicenseDialog::onActivate() {
    const QString key = keyInput_->text().trimmed();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("License"),
                             tr("Please enter a license key."));
        return;
    }
    setUiBusy(true);
    manager_->activateKey(key);
}

void LicenseDialog::onDeactivate() {
    const QString key = keyInput_->text().trimmed();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("License"),
                             tr("Please enter a license key."));
        return;
    }
    setUiBusy(true);
    manager_->deactivateKey(key);
}

void LicenseDialog::onCheckStatus() {
    const QString key = keyInput_->text().trimmed();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("License"),
                             tr("Please enter a license key."));
        return;
    }
    setUiBusy(true);
    manager_->checkStatus(key);
}

void LicenseDialog::onActivationSucceeded(const QString& message) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::information(this, tr("License Activated"), message);
}

void LicenseDialog::onActivationFailed(const QString& error) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::critical(this, tr("Activation Failed"), error);
}

void LicenseDialog::onValidationCompleted(bool valid,
                                           const QString& message) {
    setUiBusy(false);
    updateStatusDisplay();
    if (valid)
        QMessageBox::information(this, tr("License Valid"), message);
    else
        QMessageBox::warning(this, tr("License Invalid"), message);
}

void LicenseDialog::onDeactivationSucceeded(const QString& message) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::information(this, tr("License Deactivated"), message);
}

void LicenseDialog::onDeactivationFailed(const QString& error) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::critical(this, tr("Deactivation Failed"), error);
}

void LicenseDialog::onStatusChecked(LicenseManager::LicenseStatus /*status*/,
                                     const QString& message) {
    setUiBusy(false);
    updateStatusDisplay();
    QMessageBox::information(this, tr("License Status"), message);
}

void LicenseDialog::onNetworkError(const QString& error) {
    setUiBusy(false);
    QMessageBox::critical(this, tr("Network Error"), error);
}

// ── Helpers ─────────────────────────────────────────────────────────

void LicenseDialog::setUiBusy(bool busy) {
    activateButton_->setEnabled(!busy);
    deactivateButton_->setEnabled(!busy);
    checkButton_->setEnabled(!busy);
    keyInput_->setEnabled(!busy);
    if (busy)
        statusLabel_->setText(tr("Contacting server…"));
}

void LicenseDialog::updateStatusDisplay() {
    switch (manager_->status()) {
    case LicenseManager::LicenseStatus::Valid:
        statusLabel_->setText(tr("✔ Licensed"));
        statusLabel_->setStyleSheet("font-weight: bold; color: #2ea043;");
        break;
    case LicenseManager::LicenseStatus::Expired:
        statusLabel_->setText(tr("⚠ Expired"));
        statusLabel_->setStyleSheet("font-weight: bold; color: #d29922;");
        break;
    case LicenseManager::LicenseStatus::Suspended:
        statusLabel_->setText(tr("⛔ Suspended"));
        statusLabel_->setStyleSheet("font-weight: bold; color: #f85149;");
        break;
    case LicenseManager::LicenseStatus::Invalid:
        statusLabel_->setText(tr("✘ Invalid"));
        statusLabel_->setStyleSheet("font-weight: bold; color: #f85149;");
        break;
    case LicenseManager::LicenseStatus::Unknown:
    default:
        statusLabel_->setText(tr("Not activated"));
        statusLabel_->setStyleSheet("font-weight: bold; color: #8b949e;");
        break;
    }
}

} // namespace visioncast_ui
