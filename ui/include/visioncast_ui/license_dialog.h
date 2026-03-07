#pragma once

/// @file license_dialog.h
/// @brief Dialog for activating, validating, and managing a license key.

#include <QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

namespace visioncast_ui {

class LicenseManager;

/// Modal dialog that lets the user enter a license key and manage its
/// activation state.
class LicenseDialog : public QDialog {
    Q_OBJECT

public:
    explicit LicenseDialog(LicenseManager* manager,
                           QWidget* parent = nullptr);

private slots:
    void onActivate();
    void onDeactivate();
    void onCheckStatus();

    void onActivationSucceeded(const QString& message);
    void onActivationFailed(const QString& error);
    void onValidationCompleted(bool valid, const QString& message);
    void onDeactivationSucceeded(const QString& message);
    void onDeactivationFailed(const QString& error);
    void onStatusChecked(LicenseManager::LicenseStatus status,
                         const QString& message);
    void onNetworkError(const QString& error);

private:
    void setupUi();
    void connectSignals();
    void setUiBusy(bool busy);
    void updateStatusDisplay();

    LicenseManager* manager_;

    QLineEdit*   keyInput_         = nullptr;
    QLabel*      statusLabel_      = nullptr;
    QLabel*      machineIdLabel_   = nullptr;
    QPushButton* activateButton_   = nullptr;
    QPushButton* deactivateButton_ = nullptr;
    QPushButton* checkButton_      = nullptr;
    QPushButton* closeButton_      = nullptr;
};

} // namespace visioncast_ui
