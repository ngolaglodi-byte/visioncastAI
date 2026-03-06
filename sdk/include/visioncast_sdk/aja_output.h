#pragma once

/// @file aja_output.h
/// @brief AJA playout-only device wrapper.
///
/// Wraps AJADevice for output-only use cases.
/// Requires the AJA NTV2 SDK to be installed in sdk/aja/.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// Reference signal source for AJA genlock.
enum class AJAReference {
    FREE_RUN,
    EXTERNAL,
    INPUT
};

/// AJA playout-only device.
///
/// Supports multi-channel SDI output through the AJA NTV2 SDK.
/// Implements the IVideoDevice interface restricted to playout operations.
class AJAOutput : public IVideoDevice {
public:
    AJAOutput();
    ~AJAOutput() override;

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

    // -- AJAOutput-specific --

    /// Select the NTV2 channel index (0-based).
    void setChannel(int channel);

    /// Return the current NTV2 channel index.
    int channel() const;

    /// Select the reference signal source for genlock.
    void setReferenceSource(AJAReference ref);

    /// Return the current reference source.
    AJAReference referenceSource() const;

    /// Check whether the output is actively sending frames.
    bool isOutputActive() const;

    /// Enumerate all AJA playout devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
