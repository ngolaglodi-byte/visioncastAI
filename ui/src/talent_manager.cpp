/// @file talent_manager.cpp
/// @brief TalentManager implementation.

#include "visioncast_ui/talent_manager.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

namespace visioncast_ui {

TalentManager::TalentManager(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);

    model_ = new QStandardItemModel(0, 3, this);
    model_->setHorizontalHeaderLabels({"ID", "Name", "Role"});

    talentTable_ = new QTableView(this);
    talentTable_->setModel(model_);
    talentTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(talentTable_);

    auto* buttonLayout = new QHBoxLayout();
    addButton_ = new QPushButton("Add", this);
    editButton_ = new QPushButton("Edit", this);
    removeButton_ = new QPushButton("Remove", this);
    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(removeButton_);
    layout->addLayout(buttonLayout);

    setLayout(layout);
}

void TalentManager::loadDatabase(const QString& path) {
    // TODO: Parse talents.json and populate model
}

void TalentManager::saveDatabase(const QString& path) {
    // TODO: Serialize model back to talents.json
}

} // namespace visioncast_ui
