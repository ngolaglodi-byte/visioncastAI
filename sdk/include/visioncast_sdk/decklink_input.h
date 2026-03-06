#pragma once

/// @file decklink_input.h
/// @brief Blackmagic DeckLink capture-only device (SDI/HDMI, 1080p/4K).
///
/// Wraps DeckLinkDevice for input-only use cases.
/// Requires the Blackmagic DeckLink SDK to be installed in sdk/decklink/.

#include "visioncast_sdk/video_device.h"

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
class DeckLinkInput : public IVideoDevice {
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

    // -- DeckLinkInput-specific --

    /// Select the physical connector (SDI / HDMI / AUTO).
    void setConnector(DeckLinkConnector connector);

    /// Return the current connector setting.
    DeckLinkConnector connector() const;

    /// Check whether a valid signal is detected on the input.
    bool hasSignal() const;

    /// Auto-detect the incoming video mode from the signal.
    VideoMode detectedMode() const;

    /// Enumerate all DeckLink capture devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
