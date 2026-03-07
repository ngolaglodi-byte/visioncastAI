#pragma once

/// @file decklink_output.h
/// @brief Blackmagic DeckLink playout-only device (SDI/HDMI, 1080p/4K).
///
/// Wraps DeckLinkDevice for output-only use cases.
/// Requires the Blackmagic DeckLink SDK to be installed in sdk/decklink/.
///
/// Also implements VideoOutputInterface for unified output access.

#include "visioncast_sdk/video_device.h"
#include "visioncast_sdk/video_output_interface.h"
#include "visioncast_sdk/decklink_input.h"   // DeckLinkConnector enum

#ifdef HAS_DECKLINK
#include <DeckLinkAPI.h>
#endif

#include <memory>
#include <string>
#include <vector>

/// Reference signal source for genlock.
enum class DeckLinkReference {
    FREE_RUN,
    EXTERNAL,
    INPUT
};

/// Blackmagic DeckLink playout-only device.
///
/// Supports 1080p and 4K output over SDI or HDMI connectors.
/// Implements the IVideoDevice interface restricted to playout operations.
/// Also implements VideoOutputInterface for unified output access.
class DeckLinkOutput : public IVideoDevice, public VideoOutputInterface {
public:
    DeckLinkOutput();
    ~DeckLinkOutput() override;

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

    // -- VideoOutputInterface --
    bool openOutput(const DeviceConfig& config) override;
    void closeOutput() override;
    bool isOutputOpen() const override;
    bool isOutputActive() const override;
    std::string outputDeviceName() const override;
    std::vector<VideoMode> supportedOutputModes() const override;
    void setOutputTimecode(const std::string& timecode) override;

    // -- DeckLinkOutput-specific --

    /// Select the physical connector (SDI / HDMI / AUTO).
    void setConnector(DeckLinkConnector connector);

    /// Return the current connector setting.
    DeckLinkConnector connector() const;

    /// Select the reference signal source for genlock.
    void setReferenceSource(DeckLinkReference ref);

    /// Return the current reference source.
    DeckLinkReference referenceSource() const;

    /// Enable or disable low-latency output mode.
    void setLowLatency(bool enabled);

    /// Return true if low-latency mode is enabled.
    bool lowLatency() const;

    /// Enumerate all DeckLink playout devices on the system.
    static std::vector<DeviceConfig> enumerateDevices();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
