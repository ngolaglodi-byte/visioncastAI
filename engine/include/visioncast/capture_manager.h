#pragma once

/// @file capture_manager.h
/// @brief Manages video input sources (cameras, SDI, NDI, file playback).

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "visioncast_sdk/video_device.h"

namespace visioncast {

/// Manages multiple video capture sources and provides frame access.
class CaptureManager {
public:
    CaptureManager();
    ~CaptureManager();

    /// Register a named video source.
    bool addSource(const std::string& name, std::shared_ptr<IVideoDevice> device);

    /// Remove a registered source.
    bool removeSource(const std::string& name);

    /// List all registered source names.
    std::vector<std::string> listSources() const;

    /// Set the active capture source.
    void setActiveSource(const std::string& name);

    /// Capture a single frame from the active source.
    VideoFrame captureActiveFrame();

private:
    std::map<std::string, std::shared_ptr<IVideoDevice>> sources_;
    std::string activeSource_;
    mutable std::mutex mutex_;
};

} // namespace visioncast
