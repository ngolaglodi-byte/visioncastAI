/// @file output_manager.cpp
/// @brief OutputManager implementation.

#include "visioncast/output_manager.h"

namespace visioncast {

OutputManager::OutputManager() = default;
OutputManager::~OutputManager() = default;

bool OutputManager::addOutput(const std::string& name,
                              std::shared_ptr<IVideoDevice> device) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (outputs_.count(name) > 0) {
        return false;
    }
    outputs_[name] = std::move(device);
    return true;
}

bool OutputManager::removeOutput(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    return outputs_.erase(name) > 0;
}

void OutputManager::sendFrame(const VideoFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : outputs_) {
        pair.second->sendFrame(frame);
    }
}

void OutputManager::sendFrame(const std::string& outputName,
                              const VideoFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = outputs_.find(outputName);
    if (it != outputs_.end()) {
        it->second->sendFrame(frame);
    }
}

void OutputManager::startRecording(const std::string& path,
                                   const EncoderConfig& config) {
    // TODO: Initialize FFmpeg encoder for file recording
}

void OutputManager::stopRecording() {
    // TODO: Finalize and close recording file
}

} // namespace visioncast
