/// @file capture_manager.cpp
/// @brief CaptureManager implementation.

#include "visioncast/capture_manager.h"

namespace visioncast {

CaptureManager::CaptureManager() = default;
CaptureManager::~CaptureManager() = default;

bool CaptureManager::addSource(const std::string& name,
                               std::shared_ptr<IVideoDevice> device) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (sources_.count(name) > 0) {
        return false;
    }
    sources_[name] = std::move(device);
    if (activeSource_.empty()) {
        activeSource_ = name;
    }
    return true;
}

bool CaptureManager::removeSource(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sources_.find(name);
    if (it == sources_.end()) {
        return false;
    }
    sources_.erase(it);
    if (activeSource_ == name) {
        activeSource_ = sources_.empty() ? "" : sources_.begin()->first;
    }
    return true;
}

std::vector<std::string> CaptureManager::listSources() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(sources_.size());
    for (const auto& pair : sources_) {
        names.push_back(pair.first);
    }
    return names;
}

void CaptureManager::setActiveSource(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (sources_.count(name) > 0) {
        activeSource_ = name;
    }
}

VideoFrame CaptureManager::captureActiveFrame() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeSource_.empty() || sources_.count(activeSource_) == 0) {
        return VideoFrame{};
    }
    return sources_[activeSource_]->captureFrame();
}

} // namespace visioncast
