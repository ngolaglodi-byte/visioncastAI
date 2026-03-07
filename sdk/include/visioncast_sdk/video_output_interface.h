#pragma once

/// @file video_output_interface.h
/// @brief Unified video output (playout/send) interface.
///
/// Provides a role-specific abstract interface for all video output devices,
/// including hardware playout (DeckLink, AJA) and network send (NDI, SRT, RTMP).
/// This is a narrower contract than IVideoDevice, exposing only playout operations.

#include "visioncast_sdk/video_device.h"

#include <string>
#include <vector>

/// Unified abstract interface for all video output (playout/send) devices.
///
/// Implementations: DeckLinkOutput, AJAOutput, NDIOutput, SRTOutput, RTMPOutput.
class VideoOutputInterface {
public:
    virtual ~VideoOutputInterface() = default;

    // -- Lifecycle --

    /// Open the output device with the given configuration.
    virtual bool openOutput(const DeviceConfig& config) = 0;

    /// Close the output device and release all resources.
    virtual void closeOutput() = 0;

    /// Check whether the output device is currently open.
    virtual bool isOutputOpen() const = 0;

    // -- Playout --

    /// Start playout in the specified video mode.
    virtual bool startPlayout(const VideoMode& mode) = 0;

    /// Stop playout.
    virtual bool stopPlayout() = 0;

    /// Send a single frame for playout.
    virtual bool sendFrame(const VideoFrame& frame) = 0;

    // -- Status --

    /// Check whether the output is actively sending frames.
    virtual bool isOutputActive() const = 0;

    // -- Information --

    /// Return the human-readable device name.
    virtual std::string outputDeviceName() const = 0;

    /// Return the list of video modes supported by this output.
    virtual std::vector<VideoMode> supportedOutputModes() const = 0;

    // -- Timecode --

    /// Embed timecode in the next outgoing frame.
    /// Format: "HH:MM:SS:FF" (SMPTE timecode).
    virtual void setOutputTimecode(const std::string& timecode) = 0;
};
