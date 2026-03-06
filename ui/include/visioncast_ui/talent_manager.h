#pragma once

/// @file talent_manager.h
/// @brief Talent database management interface with detail panel.

#include <QWidget>

class QTableView;
class QStandardItemModel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QLabel;
class QSpinBox;
class QGroupBox;

namespace visioncast_ui {

/// CRUD interface for managing the talent face database.
///
/// Provides a table listing all talents and a detail panel for editing
/// individual talent properties: name, role, organisation, photo, overlay,
/// colours (theme_color), and animations (entry, exit, duration).
class TalentManager : public QWidget {
    Q_OBJECT

public:
    explicit TalentManager(QWidget* parent = nullptr);

    void loadDatabase(const QString& path);
    void saveDatabase(const QString& path);

signals:
    void databaseUpdated();

private slots:
    void onSelectionChanged();
    void onAddTalent();
    void onRemoveTalent();
    void onApplyDetails();

private:
    void setupTable(QVBoxLayout* parent);
    void setupDetailPanel(QVBoxLayout* parent);
    void clearDetailPanel();
    int selectedRow() const;

    // --- Table ---
    QTableView* talentTable_ = nullptr;
    QStandardItemModel* model_ = nullptr;
    QPushButton* addButton_ = nullptr;
    QPushButton* removeButton_ = nullptr;

    // --- Detail panel ---
    QGroupBox* detailGroup_ = nullptr;
    QLineEdit* nameEdit_ = nullptr;
    QLineEdit* roleEdit_ = nullptr;
    QLineEdit* orgEdit_ = nullptr;
    QLineEdit* photoEdit_ = nullptr;
    QLabel* photoPreview_ = nullptr;
    QComboBox* overlayCombo_ = nullptr;
    QLineEdit* colorEdit_ = nullptr;
    QLabel* colorSwatch_ = nullptr;
    QComboBox* entryAnimCombo_ = nullptr;
    QComboBox* exitAnimCombo_ = nullptr;
    QSpinBox* animDurationSpin_ = nullptr;
    QPushButton* applyButton_ = nullptr;

    QString dbPath_;
};

} // namespace visioncast_ui
