#pragma once

/// @file output_manager.h
/// @brief Routes processed frames to broadcast outputs and recording.

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "visioncast_sdk/video_device.h"

namespace visioncast {

/// Encoder configuration for recording.
struct EncoderConfig {
    std::string codec = "h265";   // h264, h265, prores
    int bitrateMbps = 50;
    std::string container = "mov"; // mov, mp4, mxf
};

/// Routes processed frames to broadcast output devices and recording.
class OutputManager {
public:
    OutputManager();
    ~OutputManager();

    /// Register a named output device.
    bool addOutput(const std::string& name, std::shared_ptr<IVideoDevice> device);

    /// Remove a registered output.
    bool removeOutput(const std::string& name);

    /// Send a frame to all registered outputs.
    void sendFrame(const VideoFrame& frame);

    /// Send a frame to a specific output.
    void sendFrame(const std::string& outputName, const VideoFrame& frame);

    /// Start file recording.
    void startRecording(const std::string& path, const EncoderConfig& config);

    /// Stop file recording.
    void stopRecording();

private:
    std::map<std::string, std::shared_ptr<IVideoDevice>> outputs_;
    std::mutex mutex_;
};

} // namespace visioncast
