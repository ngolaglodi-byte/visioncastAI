#pragma once

/// @file metadata_receiver.h
/// @brief Receives face recognition metadata from the Python AI module via ZeroMQ.

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "visioncast/overlay_renderer.h"

namespace visioncast {

/// Receives face recognition metadata from the Python AI module over ZeroMQ PUB/SUB.
class MetadataReceiver {
public:
    /// @param endpoint ZeroMQ subscriber endpoint (default: tcp://127.0.0.1:5555).
    explicit MetadataReceiver(const std::string& endpoint = "tcp://127.0.0.1:5555");
    ~MetadataReceiver();

    /// Start listening for metadata in a background thread.
    void start();

    /// Stop the listener thread.
    void stop();

    /// Get the latest received recognition metadata (thread-safe).
    RecognitionMetadata getLatestMetadata() const;

    /// Check if new metadata has been received since the last read.
    bool hasNewMetadata() const;

private:
    std::string endpoint_;
    std::thread listenerThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> hasNew_{false};
    mutable std::mutex dataMutex_;
    RecognitionMetadata latestMetadata_;

    void listenerLoop();
    RecognitionMetadata parseJson(const std::string& json);
};

} // namespace visioncast
