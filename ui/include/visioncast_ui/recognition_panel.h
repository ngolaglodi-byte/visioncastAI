#pragma once

/// @file recognition_panel.h
/// @brief AI recognition panel displaying detected faces, talent, confidence, and history.

#include <QWidget>
#include <QString>
#include <QDateTime>
#include <vector>

class QLabel;
class QListWidget;
class QProgressBar;

namespace visioncast_ui {

/// Snapshot of a single recognition event.
struct RecognitionEvent {
    QString faceName;
    QString talentName;
    QString role;
    double confidence = 0.0;
    QDateTime timestamp;
};

/// Displays real-time AI recognition results: detected face,
/// matched talent, confidence score, and a scrollable history log.
class RecognitionPanel : public QWidget {
    Q_OBJECT

public:
    explicit RecognitionPanel(QWidget* parent = nullptr);

    /// Push a new recognition result into the panel and history.
    void updateRecognition(const RecognitionEvent& event);

    /// Clear all current labels and history.
    void clearHistory();

    /// Maximum entries kept in the history list (default 50).
    int maxHistorySize() const;
    void setMaxHistorySize(int size);

signals:
    /// Emitted each time a new recognition event is received.
    void recognitionReceived(const RecognitionEvent& event);

private:
    QLabel* faceLabel_ = nullptr;
    QLabel* talentLabel_ = nullptr;
    QLabel* roleLabel_ = nullptr;
    QLabel* confidenceLabel_ = nullptr;
    QProgressBar* confidenceBar_ = nullptr;
    QListWidget* historyList_ = nullptr;
    int maxHistorySize_ = 50;
};

} // namespace visioncast_ui
