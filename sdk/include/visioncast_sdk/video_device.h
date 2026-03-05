#pragma once

/// @file video_device.h
/// @brief Abstract base interface for professional video capture/playout devices.
///
/// Provides a unified API for Blackmagic DeckLink, AJA, Magewell, and NDI devices.
/// See docs/ARCHITECTURE.md Section 6 for the full specification.

#include <cstdint>
#include <string>
#include <vector>

/// Pixel format for video frames.
enum class PixelFormat {
    BGRA8,  ///< 8-bit BGRA (GPU-friendly)
    UYVY,   ///< 4:2:2 packed YCbCr (SDI standard)
    V210,   ///< 10-bit 4:2:2 packed YCbCr (broadcast standard)
    NV12    ///< 4:2:0 semi-planar (encoding)
};

/// A single video frame with pixel data and metadata.
struct VideoFrame {
    uint8_t* data = nullptr;      ///< Pixel data buffer
    int width = 0;                ///< Frame width in pixels
    int height = 0;               ///< Frame height in pixels
    int stride = 0;               ///< Bytes per row
    PixelFormat format = PixelFormat::BGRA8;
    int64_t timestampUs = 0;      ///< Presentation timestamp (microseconds)
    int frameNumber = 0;          ///< Sequential frame counter
};

/// Video mode (resolution + frame rate + format).
struct VideoMode {
    int width = 1920;
    int height = 1080;
    double frameRate = 25.0;
    PixelFormat format = PixelFormat::UYVY;
    bool interlaced = false;
};

/// Device configuration for opening a specific hardware device.
struct DeviceConfig {
    int deviceIndex = 0;
    int subDeviceIndex = 0;
    std::string name;
};

/// Device capability type.
enum class DeviceType {
    CAPTURE,
    PLAYOUT,
    BIDIRECTIONAL
};

/// Abstract base class for all video capture/playout devices.
class IVideoDevice {
public:
    virtual ~IVideoDevice() = default;

    // Lifecycle
    virtual bool open(const DeviceConfig& config) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    // Capabilities
    virtual std::string deviceName() const = 0;
    virtual DeviceType deviceType() const = 0;
    virtual std::vector<VideoMode> supportedModes() const = 0;

    // Capture
    virtual bool startCapture(const VideoMode& mode) = 0;
    virtual bool stopCapture() = 0;
    virtual VideoFrame captureFrame() = 0;

    // Playout
    virtual bool startPlayout(const VideoMode& mode) = 0;
    virtual bool stopPlayout() = 0;
    virtual bool sendFrame(const VideoFrame& frame) = 0;

    // Configuration
    virtual void setVideoMode(const VideoMode& mode) = 0;
    virtual VideoMode currentMode() const = 0;
};
