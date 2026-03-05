#pragma once

/// @file overlay_editor.h
/// @brief Overlay template editor for lower-thirds and graphics.

#include <QWidget>

class QLineEdit;
class QComboBox;
class QPushButton;

namespace visioncast_ui {

/// Template editor for configuring lower-third overlays and graphics.
class OverlayEditor : public QWidget {
    Q_OBJECT

public:
    explicit OverlayEditor(QWidget* parent = nullptr);

    void loadTemplate(const QString& path);
    void saveTemplate(const QString& path);

signals:
    void templateChanged();

private:
    QLineEdit* titleEdit_ = nullptr;
    QLineEdit* subtitleEdit_ = nullptr;
    QComboBox* styleCombo_ = nullptr;
    QPushButton* applyButton_ = nullptr;
};

} // namespace visioncast_ui
