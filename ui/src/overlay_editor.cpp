/// @file overlay_editor.cpp
/// @brief OverlayEditor implementation.

#include "visioncast_ui/overlay_editor.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

namespace visioncast_ui {

OverlayEditor::OverlayEditor(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QFormLayout(this);

    titleEdit_ = new QLineEdit(this);
    subtitleEdit_ = new QLineEdit(this);
    styleCombo_ = new QComboBox(this);
    styleCombo_->addItems({"Default", "Minimal", "Bold", "News"});

    applyButton_ = new QPushButton("Apply", this);

    layout->addRow("Title:", titleEdit_);
    layout->addRow("Subtitle:", subtitleEdit_);
    layout->addRow("Style:", styleCombo_);
    layout->addRow(applyButton_);

    connect(applyButton_, &QPushButton::clicked, this, [this]() {
        emit templateChanged();
    });

    setLayout(layout);
}

void OverlayEditor::loadTemplate(const QString& path) {
    // TODO: Load template from JSON file
}

void OverlayEditor::saveTemplate(const QString& path) {
    // TODO: Save template to JSON file
}

} // namespace visioncast_ui
