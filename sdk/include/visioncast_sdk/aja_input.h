#pragma once

/// @file aja_input.h
/// @brief AJA capture-only device wrapper.
///
/// Wraps AJADevice for input-only use cases.
/// Requires the AJA NTV2 SDK to be installed in sdk/aja/.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// AJA capture-only device.
///
/// Supports multi-channel SDI capture through the AJA NTV2 SDK.
/// Implements the IVideoDevice interface restricted to capture operations.
class AJAInput : public IVideoDevice {
public:
    AJAInput();
    ~AJAInput() override;

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

    // -- AJAInput-specific --

    /// Select the NTV2 channel index (0-based).
    void setChannel(int channel);

    /// Return the current NTV2 channel index.
    int channel() const;

    /// Check whether a valid signal is detected on the input.
    bool hasSignal() const;

    /// Auto-detect the incoming video mode from the signal.
    VideoMode detectedMode() const;

    /// Enumerate all AJA capture devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
