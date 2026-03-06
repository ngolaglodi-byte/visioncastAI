/// @file recognition_panel.cpp
/// @brief RecognitionPanel implementation.

#include "visioncast_ui/recognition_panel.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QVBoxLayout>

namespace visioncast_ui {

RecognitionPanel::RecognitionPanel(QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);

    // --- Current recognition group ---
    auto* currentGroup = new QGroupBox("Current Recognition", this);
    auto* form = new QFormLayout(currentGroup);

    faceLabel_ = new QLabel("—", this);
    talentLabel_ = new QLabel("—", this);
    roleLabel_ = new QLabel("—", this);
    confidenceLabel_ = new QLabel("0 %", this);
    confidenceBar_ = new QProgressBar(this);
    confidenceBar_->setRange(0, 100);
    confidenceBar_->setValue(0);

    form->addRow("Detected Face:", faceLabel_);
    form->addRow("Talent:", talentLabel_);
    form->addRow("Role:", roleLabel_);
    form->addRow("Confidence:", confidenceBar_);
    form->addRow("", confidenceLabel_);

    currentGroup->setLayout(form);
    root->addWidget(currentGroup);

    // --- History group ---
    auto* historyGroup = new QGroupBox("History", this);
    auto* histLayout = new QVBoxLayout(historyGroup);
    historyList_ = new QListWidget(this);
    histLayout->addWidget(historyList_);
    historyGroup->setLayout(histLayout);
    root->addWidget(historyGroup);

    setLayout(root);
}

void RecognitionPanel::updateRecognition(const RecognitionEvent& event) {
    faceLabel_->setText(event.faceName.isEmpty() ? "—" : event.faceName);
    talentLabel_->setText(event.talentName.isEmpty() ? "—" : event.talentName);
    roleLabel_->setText(event.role.isEmpty() ? "—" : event.role);

    int pct = static_cast<int>(event.confidence * 100.0);
    confidenceBar_->setValue(pct);
    confidenceLabel_->setText(QString::number(event.confidence * 100.0, 'f', 1) + " %");

    // Build history entry
    QString ts = event.timestamp.isValid()
        ? event.timestamp.toString("hh:mm:ss")
        : QDateTime::currentDateTime().toString("hh:mm:ss");

    QString talent = event.talentName.isEmpty() ? "Unknown" : event.talentName;
    QString pctStr = QString::number(event.confidence * 100.0, 'f', 1) + "%";

    QString entry;
    if (event.role.isEmpty()) {
        entry = QString("[%1] %2 (%3)").arg(ts, talent, pctStr);
    } else {
        entry = QString("[%1] %2 — %3 (%4)").arg(ts, talent, event.role, pctStr);
    }

    historyList_->insertItem(0, entry);

    // Trim history
    while (historyList_->count() > maxHistorySize_) {
        delete historyList_->takeItem(historyList_->count() - 1);
    }

    emit recognitionReceived(event);
}

void RecognitionPanel::clearHistory() {
    faceLabel_->setText("—");
    talentLabel_->setText("—");
    roleLabel_->setText("—");
    confidenceLabel_->setText("0 %");
    confidenceBar_->setValue(0);
    historyList_->clear();
}

int RecognitionPanel::maxHistorySize() const {
    return maxHistorySize_;
}

void RecognitionPanel::setMaxHistorySize(int size) {
    maxHistorySize_ = size > 0 ? size : 1;
    while (historyList_->count() > maxHistorySize_) {
        delete historyList_->takeItem(historyList_->count() - 1);
    }
}

} // namespace visioncast_ui
