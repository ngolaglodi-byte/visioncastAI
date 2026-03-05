#pragma once

/// @file ndi_device.h
/// @brief NDI (Network Device Interface) device wrapper.
///
/// Requires the NDI SDK to be installed in sdk/ndi/.

#include "visioncast_sdk/video_device.h"

#include <memory>

/// NDI network video device implementation.
class NDIDevice : public IVideoDevice {
public:
    NDIDevice();
    ~NDIDevice() override;

    bool open(const DeviceConfig& config) override;
    void close() override;
    bool isOpen() const override;

    std::string deviceName() const override;
    DeviceType deviceType() const override;
    std::vector<VideoMode> supportedModes() const override;

    bool startCapture(const VideoMode& mode) override;
    bool stopCapture() override;
    VideoFrame captureFrame() override;

    bool startPlayout(const VideoMode& mode) override;
    bool stopPlayout() override;
    bool sendFrame(const VideoFrame& frame) override;

    void setVideoMode(const VideoMode& mode) override;
    VideoMode currentMode() const override;

    /// Discover available NDI sources on the network.
    static std::vector<DeviceConfig> discoverSources();

    /// Set the NDI source name for sending.
    void setSourceName(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
