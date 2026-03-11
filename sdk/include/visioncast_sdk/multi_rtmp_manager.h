#pragma once

/// @file multi_rtmp_manager.h
/// @brief Multi-platform RTMP streaming manager.
///
/// Manages a dynamic list of concurrent RTMP output streams, each targeting
/// an independent platform (YouTube Live, Facebook Live, Twitch, etc.).
/// All public methods are thread-safe.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast_sdk/rtmp_output.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

// ── Stream status ──────────────────────────────────────────────────────────

/// Lifecycle state of one managed RTMP stream.
enum class RtmpStreamStatus {
    Idle,        ///< Not started; configuration only.
    Connecting,  ///< Opening the RTMP connection (async).
    Live,        ///< Connected and streaming.
    Error        ///< Last start attempt failed.
};

/// Convert RtmpStreamStatus to a human-readable string.
const char* rtmpStreamStatusToString(RtmpStreamStatus s);

// ── Stream entry ───────────────────────────────────────────────────────────

/// Snapshot of configuration and runtime state for one RTMP stream.
///
/// This is a plain data struct returned by value from MultiRtmpManager::streams()
/// and MultiRtmpManager::getStream().  It is not the live object — mutating it
/// has no effect on the manager.
struct RtmpStreamEntry {
    std::string      id;             ///< Unique identifier (UUID-style string).
    std::string      name;           ///< Display name (e.g. "YouTube Live").
    std::string      platform;       ///< Platform hint: "youtube","facebook","twitch","custom".
    std::string      serverUrl;      ///< RTMP server URL.
    std::string      streamKey;      ///< Stream / publish key.
    RtmpStreamStatus status          = RtmpStreamStatus::Idle;
    std::string      statusMessage;  ///< Last status message (error text, etc.).
    std::vector<std::string> logLines; ///< Recent log lines (capped at kMaxLogLines).

    static constexpr int kMaxLogLines = 50;
};

// ── Manager ────────────────────────────────────────────────────────────────

/// Manages multiple concurrent RTMP output streams.
///
/// Each stream is handled independently: it has its own RTMPOutput instance,
/// worker thread, and mutex.  The manager itself also holds a global mutex
/// that protects the stream list.
///
/// Typical use:
/// @code
///   MultiRtmpManager mgr;
///   mgr.setStatusCallback([](auto& id, auto st, auto& msg){ … });
///   auto id = mgr.addStream("YouTube Live", "youtube",
///                           "rtmp://a.rtmp.youtube.com/live2", "MY_KEY");
///   mgr.startStream(id);
///   // … later …
///   mgr.stopStream(id);
/// @endcode
class MultiRtmpManager {
public:
    /// Callback invoked on every stream status change.
    /// @note May be called from a worker thread.
    using StatusCallback = std::function<void(const std::string& id,
                                              RtmpStreamStatus   status,
                                              const std::string& message)>;

    MultiRtmpManager();
    ~MultiRtmpManager();

    // Non-copyable, non-movable
    MultiRtmpManager(const MultiRtmpManager&)            = delete;
    MultiRtmpManager& operator=(const MultiRtmpManager&) = delete;

    // ── Callbacks ───────────────────────────────────────────────────

    /// Register a callback for status-change events (replaces any previous one).
    void setStatusCallback(StatusCallback cb);

    // ── Stream management ───────────────────────────────────────────

    /// Add a new stream configuration.
    /// @return The generated stream ID.
    std::string addStream(const std::string& name,
                          const std::string& platform,
                          const std::string& serverUrl,
                          const std::string& streamKey);

    /// Remove a stream.  Stops it first if it is currently Live/Connecting.
    /// @return false if the ID was not found.
    bool removeStream(const std::string& id);

    /// Update stream configuration.
    /// Only allowed when the stream is Idle or Error.
    /// @return false if the ID was not found, or the stream is currently active.
    bool updateStream(const std::string& id,
                      const std::string& name,
                      const std::string& serverUrl,
                      const std::string& streamKey);

    // ── Lifecycle ───────────────────────────────────────────────────

    /// Start a stream asynchronously.
    /// Status transitions: Idle/Error → Connecting → Live (or Error on failure).
    /// @return false if the ID was not found or the stream is already active.
    bool startStream(const std::string& id);

    /// Stop a stream.  Blocks until the worker thread finishes.
    /// Status transitions: Live/Connecting → Idle.
    /// @return false if the ID was not found.
    bool stopStream(const std::string& id);

    /// Stop all streams concurrently, then join all worker threads.
    void stopAll();

    // ── Queries ─────────────────────────────────────────────────────

    /// Return a snapshot copy of all stream entries.
    std::vector<RtmpStreamEntry> streams() const;

    /// Populate @p out with the current state of a single stream.
    /// @return false if the ID was not found.
    bool getStream(const std::string& id, RtmpStreamEntry& out) const;

    /// Return the number of currently managed streams.
    std::size_t streamCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
