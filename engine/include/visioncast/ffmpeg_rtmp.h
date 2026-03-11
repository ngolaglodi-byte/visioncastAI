#pragma once

/// @file ffmpeg_rtmp.h
/// @brief FFmpeg-based RTMP streaming module for the VisionCast engine.
///
/// This module provides RTMP streaming functionality using FFmpeg directly,
/// replacing the SDK RTMP implementation for a cleaner architecture.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace visioncast {

/// Configuration for an RTMP stream output.
struct RtmpStreamConfig {
    std::string serverUrl;          ///< RTMP server URL (e.g., "rtmp://live.example.com/app")
    std::string streamKey;          ///< Stream key (appended to server URL)
    int width = 1920;               ///< Video width in pixels
    int height = 1080;              ///< Video height in pixels
    double frameRate = 30.0;        ///< Frame rate (fps)
    int bitrateBps = 4000000;       ///< Video bitrate in bits per second
    std::string preset = "veryfast"; ///< H.264 encoder preset
    std::string tune = "zerolatency"; ///< H.264 encoder tune
};

/// Status of an RTMP stream.
enum class RtmpStatus {
    Idle,        ///< Not started
    Connecting,  ///< Opening RTMP connection
    Live,        ///< Connected and streaming
    Error        ///< Connection failed or error occurred
};

/// Convert RtmpStatus to a human-readable string.
const char* rtmpStatusToString(RtmpStatus status);

/// Raw video frame data for RTMP streaming.
struct RtmpFrame {
    const uint8_t* data = nullptr;  ///< Pixel data buffer (NV12 or YUV420P)
    int width = 0;                  ///< Frame width in pixels
    int height = 0;                 ///< Frame height in pixels
    int stride = 0;                 ///< Bytes per row
    bool isNV12 = true;             ///< True if NV12, false if YUV420P
};

/// Callback for status changes.
using RtmpStatusCallback = std::function<void(RtmpStatus status, const std::string& message)>;

/// FFmpeg-based RTMP streaming output.
///
/// Provides a clean interface for RTMP streaming without relying on the
/// SDK layer. Uses FFmpeg's libavformat and libavcodec for encoding and
/// streaming.
class FFmpegRtmpOutput {
public:
    FFmpegRtmpOutput();
    ~FFmpegRtmpOutput();

    // Non-copyable
    FFmpegRtmpOutput(const FFmpegRtmpOutput&) = delete;
    FFmpegRtmpOutput& operator=(const FFmpegRtmpOutput&) = delete;

    // Move-enabled
    FFmpegRtmpOutput(FFmpegRtmpOutput&&) noexcept;
    FFmpegRtmpOutput& operator=(FFmpegRtmpOutput&&) noexcept;

    /// Set the stream configuration before opening.
    void setConfig(const RtmpStreamConfig& config);

    /// Get the current configuration.
    RtmpStreamConfig config() const;

    /// Open the RTMP connection using the current configuration.
    bool open();

    /// Open the RTMP connection with a new configuration.
    bool open(const RtmpStreamConfig& config);

    /// Close the RTMP connection and release resources.
    void close();

    /// Check if the stream is open.
    bool isOpen() const;

    /// Start streaming (initializes encoder and writes stream header).
    bool start();

    /// Stop streaming (flushes encoder and writes stream trailer).
    bool stop();

    /// Check if the stream is actively sending frames.
    bool isStreaming() const;

    /// Send a video frame to the RTMP server.
    bool sendFrame(const RtmpFrame& frame);

    /// Get the current stream status.
    RtmpStatus status() const;

    /// Set a callback for status changes.
    void setStatusCallback(RtmpStatusCallback callback);

    /// Get the last error message (if status is Error).
    std::string lastError() const;

    /// Get the full RTMP URL (serverUrl + "/" + streamKey).
    std::string fullUrl() const;

    /// Get supported video resolutions.
    static std::vector<std::pair<int, int>> supportedResolutions();

    /// Get supported frame rates.
    static std::vector<double> supportedFrameRates();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace visioncast
