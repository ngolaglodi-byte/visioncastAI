#pragma once

/// @file talent_manager.h
/// @brief Talent database management interface.

#include <QWidget>

class QTableView;
class QStandardItemModel;
class QPushButton;

namespace visioncast_ui {

/// CRUD interface for managing the talent face database.
class TalentManager : public QWidget {
    Q_OBJECT

public:
    explicit TalentManager(QWidget* parent = nullptr);

    void loadDatabase(const QString& path);
    void saveDatabase(const QString& path);

signals:
    void databaseUpdated();

private:
    QTableView* talentTable_ = nullptr;
    QStandardItemModel* model_ = nullptr;
    QPushButton* addButton_ = nullptr;
    QPushButton* removeButton_ = nullptr;
    QPushButton* editButton_ = nullptr;
};

} // namespace visioncast_ui
