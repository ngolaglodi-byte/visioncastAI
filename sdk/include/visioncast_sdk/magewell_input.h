#pragma once

/// @file magewell_input.h
/// @brief Magewell capture-only device wrapper.
///
/// Wraps MagewellDevice for input-only use cases.
/// Requires the Magewell SDK to be installed in sdk/magewell/.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// Magewell capture-only device.
///
/// Supports USB and PCIe capture through the Magewell SDK.
/// Implements the IVideoDevice interface restricted to capture operations.
class MagewellInput : public IVideoDevice {
public:
    MagewellInput();
    ~MagewellInput() override;

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

    // -- IVideoDevice playout (no-ops for capture-only device) --
    bool startPlayout(const VideoMode& mode) override;
    bool stopPlayout() override;
    bool sendFrame(const VideoFrame& frame) override;

    // -- IVideoDevice configuration --
    void setVideoMode(const VideoMode& mode) override;
    VideoMode currentMode() const override;

    // -- MagewellInput-specific --

    /// Check whether a valid signal is detected on the input.
    bool hasSignal() const;

    /// Auto-detect the incoming video mode from the signal.
    VideoMode detectedMode() const;

    /// Enumerate all Magewell capture devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
