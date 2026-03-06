#pragma once

/// @file ndi_output.h
/// @brief NDI (Network Device Interface) send-only device wrapper.
///
/// Wraps NDIDevice for output-only use cases.
/// Requires the NDI SDK to be installed in sdk/ndi/.

#include "visioncast_sdk/video_device.h"

#include <memory>
#include <string>
#include <vector>

/// NDI send-only device.
///
/// Publishes an NDI stream on the local network.
/// Implements the IVideoDevice interface restricted to playout operations.
class NDIOutput : public IVideoDevice {
public:
    NDIOutput();
    ~NDIOutput() override;

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

    // -- NDIOutput-specific --

    /// Set the NDI source name visible on the network.
    void setSourceName(const std::string& name);

    /// Return the current source name.
    std::string sourceName() const;

    /// Check whether the output is actively sending frames.
    bool isOutputActive() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
