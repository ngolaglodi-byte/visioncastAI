#pragma once

/// @file ndi_input.h
/// @brief NDI (Network Device Interface) receive-only device wrapper.
///
/// Wraps NDIDevice for input-only use cases.
/// Requires the NDI SDK to be installed in sdk/ndi/.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// NDI receive-only device.
///
/// Discovers and receives NDI streams over the local network.
/// Implements the IVideoDevice interface restricted to capture operations.
class NDIInput : public IVideoDevice {
public:
    NDIInput();
    ~NDIInput() override;

    // -- IVideoDevice lifecycle --
    bool open(const DeviceConfig& config) override;
    void close() override;
    bool isOpen() const override;

    // -- IVideoDevice capabilities --
    std::string deviceName() const override;
    DeviceType deviceType() const override;
    std::vector<VideoMode> supportedModes() const override;

    // -- IVideoDevice capture --
    bool startCapture(const VideoMode& mode) override;
    bool stopCapture() override;
    VideoFrame captureFrame() override;

    // -- IVideoDevice playout (no-ops for input-only device) --
    bool startPlayout(const VideoMode& mode) override;
    bool stopPlayout() override;
    bool sendFrame(const VideoFrame& frame) override;

    // -- IVideoDevice configuration --
    void setVideoMode(const VideoMode& mode) override;
    VideoMode currentMode() const override;

    // -- NDIInput-specific --

    /// Discover available NDI sources on the network.
    static std::vector<DeviceConfig> discoverSources();

    /// Check whether a valid NDI stream is currently being received.
    bool hasSignal() const;

    /// Return the detected video mode of the incoming stream.
    VideoMode detectedMode() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
