#pragma once

/// @file zmq_receiver.h
/// @brief ZeroMQ subscriber that receives Python AI metadata and stores it
///        in a thread-safe structure for consumption by the C++ engine.

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "visioncast/overlay_renderer.h"

namespace visioncast {

/// Thread-safe store for the latest recognition metadata.
///
/// All public methods are safe to call from any thread. The receiver thread
/// calls update() while the engine thread calls get() / hasNew().
class MetadataStore {
public:
    /// Replace stored metadata (called from the receiver thread).
    void update(const RecognitionMetadata& metadata);

    /// Retrieve the latest metadata (safe to call from any thread).
    RecognitionMetadata get() const;

    /// Returns true if new data arrived since the last call to consumeNew().
    bool hasNew() const;

    /// Marks current data as consumed and returns true if there was new data.
    bool consumeNew();

private:
    mutable std::mutex mutex_;
    RecognitionMetadata latest_;
    std::atomic<bool> hasNew_{false};
};

/// ZeroMQ subscriber that receives Python AI metadata and stores it in a
/// thread-safe MetadataStore.
///
/// Usage:
/// @code
///   ZmqReceiver receiver("tcp://127.0.0.1:5556", "talent.overlay");
///   receiver.start();
///   // ... later, from the engine thread:
///   if (receiver.hasNewMetadata()) {
///       auto md = receiver.getLatestMetadata();
///       receiver.consumeNew();
///   }
///   receiver.stop();
/// @endcode
class ZmqReceiver {
public:
    /// @param endpoint ZeroMQ endpoint to connect to (default matches Python sender).
    /// @param topic    ZeroMQ subscription topic filter.
    explicit ZmqReceiver(const std::string& endpoint = "tcp://127.0.0.1:5556",
                         const std::string& topic = "talent.overlay");
    ~ZmqReceiver();

    ZmqReceiver(const ZmqReceiver&) = delete;
    ZmqReceiver& operator=(const ZmqReceiver&) = delete;

    /// Start the background listener thread.
    void start();

    /// Stop the background listener thread (blocks until joined).
    void stop();

    /// True when the listener thread is active.
    bool isRunning() const;

    /// Thread-safe access to the latest metadata.
    RecognitionMetadata getLatestMetadata() const;

    /// True if metadata arrived since the last consumeNew().
    bool hasNewMetadata() const;

    /// Marks the current data as consumed.
    bool consumeNew();

private:
    std::string endpoint_;
    std::string topic_;
    std::thread listenerThread_;
    std::atomic<bool> running_{false};
    MetadataStore store_;

    void listenerLoop();

public:
    /// Parse a JSON payload into RecognitionMetadata.
    /// Handles both "recognition_result" and "talent_overlay" message types.
    /// Public so that callers can unit-test or reuse the parser independently.
    static RecognitionMetadata parseJson(const std::string& json);
};

} // namespace visioncast
