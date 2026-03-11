#pragma once

/// @file multi_ffmpeg_rtmp_manager.h
/// @brief Multi-platform RTMP streaming manager using FFmpeg RTMP module.
///
/// Manages a dynamic list of concurrent RTMP output streams, each targeting
/// an independent platform (YouTube Live, Facebook Live, Twitch, etc.).
/// All public methods are thread-safe.
///
/// This is the new architecture replacement for the SDK's MultiRtmpManager,
/// using the engine's FFmpegRtmpOutput module instead of the SDK's RTMPOutput.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast/ffmpeg_rtmp.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace visioncast {

// ── Stream entry ───────────────────────────────────────────────────────────

/// Snapshot of configuration and runtime state for one RTMP stream.
///
/// This is a plain data struct returned by value from MultiFFmpegRtmpManager.
/// Mutating it has no effect on the manager.
struct FFmpegRtmpStreamEntry {
    std::string      id;             ///< Unique identifier.
    std::string      name;           ///< Display name (e.g. "YouTube Live").
    std::string      platform;       ///< Platform hint: "youtube","facebook","twitch","custom".
    std::string      serverUrl;      ///< RTMP server URL.
    std::string      streamKey;      ///< Stream / publish key.
    RtmpStatus       status = RtmpStatus::Idle;
    std::string      statusMessage;  ///< Last status message.
    std::vector<std::string> logLines; ///< Recent log lines (capped at kMaxLogLines).

    static constexpr int kMaxLogLines = 50;
};

// ── Manager ────────────────────────────────────────────────────────────────

/// Manages multiple concurrent RTMP output streams using FFmpeg.
///
/// Each stream has its own FFmpegRtmpOutput instance and worker thread.
///
/// Typical use:
/// @code
///   MultiFFmpegRtmpManager mgr;
///   mgr.setStatusCallback([](auto& id, auto st, auto& msg){ … });
///   auto id = mgr.addStream("YouTube Live", "youtube",
///                           "rtmp://a.rtmp.youtube.com/live2", "MY_KEY");
///   mgr.startStream(id);
///   // … later …
///   mgr.stopStream(id);
/// @endcode
class MultiFFmpegRtmpManager {
public:
    /// Callback invoked on every stream status change.
    /// @note May be called from a worker thread.
    using StatusCallback = std::function<void(const std::string& id,
                                              RtmpStatus          status,
                                              const std::string&  message)>;

    MultiFFmpegRtmpManager();
    ~MultiFFmpegRtmpManager();

    // Non-copyable, non-movable
    MultiFFmpegRtmpManager(const MultiFFmpegRtmpManager&)            = delete;
    MultiFFmpegRtmpManager& operator=(const MultiFFmpegRtmpManager&) = delete;

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

    // ── Frame sending ───────────────────────────────────────────────

    /// Send a frame to all active streams.
    void sendFrame(const RtmpFrame& frame);

    /// Send a frame to a specific stream by ID.
    bool sendFrame(const std::string& id, const RtmpFrame& frame);

    // ── Queries ─────────────────────────────────────────────────────

    /// Return a snapshot copy of all stream entries.
    std::vector<FFmpegRtmpStreamEntry> streams() const;

    /// Populate @p out with the current state of a single stream.
    /// @return false if the ID was not found.
    bool getStream(const std::string& id, FFmpegRtmpStreamEntry& out) const;

    /// Return the number of currently managed streams.
    std::size_t streamCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace visioncast
