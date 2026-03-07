#pragma once

/// @file video_input_interface.h
/// @brief Unified video input (capture/receive) interface.
///
/// Provides a role-specific abstract interface for all video input devices,
/// including hardware capture (DeckLink, AJA, Magewell) and network receive (NDI).
/// This is a narrower contract than IVideoDevice, exposing only capture operations.

#include "visioncast_sdk/video_device.h"

#include <functional>
#include <string>
#include <vector>

/// Callback invoked when a new frame is captured.
using FrameCallback = std::function<void(const VideoFrame& frame)>;

/// Unified abstract interface for all video input (capture/receive) devices.
///
/// Implementations: DeckLinkInput, AJAInput, MagewellInput, NDIInput.
class VideoInputInterface {
public:
    virtual ~VideoInputInterface() = default;

    // -- Lifecycle --

    /// Open the input device with the given configuration.
    virtual bool openInput(const DeviceConfig& config) = 0;

    /// Close the input device and release all resources.
    virtual void closeInput() = 0;

    /// Check whether the input device is currently open.
    virtual bool isInputOpen() const = 0;

    // -- Capture --

    /// Start capturing frames in the specified video mode.
    virtual bool startCapture(const VideoMode& mode) = 0;

    /// Stop capturing frames.
    virtual bool stopCapture() = 0;

    /// Synchronously capture a single frame (blocking).
    virtual VideoFrame captureFrame() = 0;

    /// Register an asynchronous frame callback for push-mode capture.
    virtual void setFrameCallback(FrameCallback callback) = 0;

    // -- Signal detection --

    /// Check whether a valid signal is detected on the input.
    virtual bool hasSignal() const = 0;

    /// Auto-detect the incoming video mode from the signal.
    virtual VideoMode detectedMode() const = 0;

    // -- Information --

    /// Return the human-readable device name.
    virtual std::string inputDeviceName() const = 0;

    /// Return the list of video modes supported by this input.
    virtual std::vector<VideoMode> supportedInputModes() const = 0;

    // -- Timecode --

    /// Return the timecode string from the most recently captured frame.
    /// Format: "HH:MM:SS:FF" (SMPTE timecode).
    virtual std::string lastTimecode() const = 0;
};
