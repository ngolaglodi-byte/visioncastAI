#pragma once

/// @file decklink_device.h
/// @brief Blackmagic DeckLink video device wrapper.
///
/// Requires the Blackmagic DeckLink SDK to be installed in sdk/decklink/.

#include "visioncast_sdk/video_device.h"

#include <memory>

/// Blackmagic DeckLink device implementation.
class DeckLinkDevice : public IVideoDevice {
public:
    DeckLinkDevice();
    ~DeckLinkDevice() override;

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

    /// Enumerate all DeckLink devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
