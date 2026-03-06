#pragma once

/// @file srt_output.h
/// @brief SRT (Secure Reliable Transport) send-only device wrapper.
///
/// Wraps an SRT sender for low-latency, reliable video streaming.
/// Requires FFmpeg with SRT support or the libsrt library.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// SRT send-only device.
///
/// Publishes an SRT stream to the configured destination.
/// Implements the IVideoDevice interface restricted to playout operations.
class SRTOutput : public IVideoDevice {
public:
    SRTOutput();
    ~SRTOutput() override;

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

    // -- SRTOutput-specific --

    /// Set the destination URL (e.g. "srt://host:port").
    void setDestination(const std::string& url);

    /// Return the current destination URL.
    std::string destination() const;

    /// Set stream latency in milliseconds.
    void setLatency(int ms);

    /// Return the configured latency in milliseconds.
    int latency() const;

    /// Check whether the output is actively sending frames.
    bool isOutputActive() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
