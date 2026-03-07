#pragma once

/// @file ndi_input.h
/// @brief NDI (Network Device Interface) receive-only device wrapper.
///
/// Wraps NDIDevice for input-only use cases.
/// Requires the NDI SDK to be installed in sdk/ndi/.
///
/// Also implements VideoInputInterface for unified input access.
/// Supports both NDI HX (compressed) and Full NDI (uncompressed) streams.

#include "visioncast_sdk/video_device.h"
#include "visioncast_sdk/video_input_interface.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

/// NDI bandwidth mode selection.
enum class NDIBandwidth {
    FULL,   ///< Full NDI — uncompressed, highest quality
    HX      ///< NDI HX — compressed, lower bandwidth
};

/// NDI receive-only device.
///
/// Discovers and receives NDI streams over the local network.
/// Implements the IVideoDevice interface restricted to capture operations.
/// Also implements VideoInputInterface for unified input access.
/// Supports NDI HX (compressed) and Full NDI (uncompressed).
class NDIInput : public IVideoDevice, public VideoInputInterface {
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

    // -- VideoInputInterface --
    bool openInput(const DeviceConfig& config) override;
    void closeInput() override;
    bool isInputOpen() const override;
    void setFrameCallback(FrameCallback callback) override;
    bool hasSignal() const override;
    VideoMode detectedMode() const override;
    std::string inputDeviceName() const override;
    std::vector<VideoMode> supportedInputModes() const override;
    std::string lastTimecode() const override;

    // -- NDIInput-specific --

    /// Discover available NDI sources on the network.
    static std::vector<DeviceConfig> discoverSources();

    /// Set the NDI bandwidth mode (Full NDI or NDI HX).
    void setBandwidth(NDIBandwidth bandwidth);

    /// Return the current bandwidth mode.
    NDIBandwidth bandwidth() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
