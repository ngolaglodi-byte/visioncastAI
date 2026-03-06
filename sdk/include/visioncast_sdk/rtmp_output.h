#pragma once

/// @file rtmp_output.h
/// @brief RTMP (Real-Time Messaging Protocol) send-only device wrapper.
///
/// Wraps an RTMP publisher for live streaming to platforms
/// such as YouTube Live, Twitch, or custom RTMP servers.
/// Requires FFmpeg with RTMP support or librtmp.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// RTMP send-only device.
///
/// Publishes an RTMP stream to the configured server URL.
/// Implements the IVideoDevice interface restricted to playout operations.
class RTMPOutput : public IVideoDevice {
public:
    RTMPOutput();
    ~RTMPOutput() override;

    // -- IVideoDevice lifecycle --
    bool open(const DeviceConfig& config) override;
    void close() override;
    bool isOpen() const override;

    // -- IVideoDevice capabilities --
    std::string deviceName() const override;
    DeviceType deviceType() const override;
    std::vector<VideoMode> supportedModes() const override;

    // -- IVideoDevice capture (no-ops for output-only device) --
    bool startCapture(const VideoMode& mode) override;
    bool stopCapture() override;
    VideoFrame captureFrame() override;

    // -- IVideoDevice playout --
    bool startPlayout(const VideoMode& mode) override;
    bool stopPlayout() override;
    bool sendFrame(const VideoFrame& frame) override;

    // -- IVideoDevice configuration --
    void setVideoMode(const VideoMode& mode) override;
    VideoMode currentMode() const override;

    // -- RTMPOutput-specific --

    /// Set the server URL (e.g. "rtmp://live.example.com/app/stream_key").
    void setServerUrl(const std::string& url);

    /// Return the current server URL.
    std::string serverUrl() const;

    /// Set the stream key (appended to server URL if not embedded).
    void setStreamKey(const std::string& key);

    /// Return the current stream key.
    std::string streamKey() const;

    /// Check whether the output is actively sending frames.
    bool isOutputActive() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
