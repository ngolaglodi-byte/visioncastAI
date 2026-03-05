/// @file preview_panel.cpp
/// @brief PreviewPanel implementation.

#include "visioncast_ui/preview_panel.h"

#include <QVBoxLayout>

namespace visioncast_ui {

PreviewPanel::PreviewPanel(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    previewLabel_ = new QLabel("No Preview", this);
    previewLabel_->setAlignment(Qt::AlignCenter);
    previewLabel_->setMinimumSize(640, 360);
    previewLabel_->setStyleSheet("background-color: black; color: white;");
    layout->addWidget(previewLabel_);
    setLayout(layout);
}

void PreviewPanel::setPreviewFrame(const QImage& frame) {
    previewLabel_->setPixmap(
        QPixmap::fromImage(frame).scaled(
            previewLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PreviewPanel::setMultiviewLayout(int rows, int cols) {
    // TODO: Create grid of preview labels for multiview
}

} // namespace visioncast_ui
