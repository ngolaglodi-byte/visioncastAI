#pragma once

/// @file decklink_input.h
/// @brief Blackmagic DeckLink capture-only device (SDI/HDMI, 1080p/4K).
///
/// Wraps DeckLinkDevice for input-only use cases.
/// Requires the Blackmagic DeckLink SDK to be installed in sdk/decklink/.
///
/// Also implements VideoInputInterface for unified input access.

#include "visioncast_sdk/video_device.h"
#include "visioncast_sdk/video_input_interface.h"

#ifdef HAS_DECKLINK
#include <DeckLinkAPI.h>
#endif

#include <functional>
#include <memory>
#include <string>
#include <vector>

/// SDI / HDMI connector selection.
enum class DeckLinkConnector {
    SDI,
    HDMI,
    AUTO
};

/// Blackmagic DeckLink capture-only device.
///
/// Supports 1080p and 4K capture over SDI or HDMI connectors.
/// Implements the IVideoDevice interface restricted to capture operations.
/// Also implements VideoInputInterface for unified input access.
class DeckLinkInput : public IVideoDevice, public VideoInputInterface {
public:
    DeckLinkInput();
    ~DeckLinkInput() override;

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

    // -- DeckLinkInput-specific --

    /// Select the physical connector (SDI / HDMI / AUTO).
    void setConnector(DeckLinkConnector connector);

    /// Return the current connector setting.
    DeckLinkConnector connector() const;

    /// Enable or disable low-latency capture mode.
    void setLowLatency(bool enabled);

    /// Return true if low-latency mode is enabled.
    bool lowLatency() const;

    /// Enumerate all DeckLink capture devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
