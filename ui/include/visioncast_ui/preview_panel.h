#pragma once

/// @file preview_panel.h
/// @brief Live video preview with multiview support.

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QImage>

#include <vector>

namespace visioncast_ui {

/// Displays live video preview with optional multiview grid layout.
class PreviewPanel : public QWidget {
    Q_OBJECT

public:
    explicit PreviewPanel(QWidget* parent = nullptr);

    /// Set the main preview frame.
    void setPreviewFrame(const QImage& frame);

    /// Configure multiview layout (e.g. 2x2, 3x3).
    void setMultiviewLayout(int rows, int cols);

signals:
    void sourceSelected(const QString& sourceName);

private:
    QLabel* previewLabel_ = nullptr;
    QGridLayout* multiviewGrid_ = nullptr;
    std::vector<QLabel*> multiviewPanels_;
};

} // namespace visioncast_ui
