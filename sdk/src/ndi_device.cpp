/// @file ndi_device.cpp
/// @brief NDI network device implementation.

#include "visioncast_sdk/ndi_device.h"

#include <iostream>

struct NDIDevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "NDI";
    std::string sourceName = "VisionCast";
};

NDIDevice::NDIDevice() : impl_(std::make_unique<Impl>()) {}
NDIDevice::~NDIDevice() { close(); }

bool NDIDevice::open(const DeviceConfig& config) {
#ifdef HAS_NDI
    // TODO: Initialize NDI SDK, create sender/receiver
    impl_->name = config.name.empty() ? "NDI" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[NDI] SDK not available." << std::endl;
    return false;
#endif
}

void NDIDevice::close() {
    impl_->isOpen = false;
}

bool NDIDevice::isOpen() const { return impl_->isOpen; }
std::string NDIDevice::deviceName() const { return impl_->name; }
DeviceType NDIDevice::deviceType() const { return DeviceType::BIDIRECTIONAL; }
std::vector<VideoMode> NDIDevice::supportedModes() const { return {}; }
bool NDIDevice::startCapture(const VideoMode& mode) { return false; }
bool NDIDevice::stopCapture() { return false; }
VideoFrame NDIDevice::captureFrame() { return VideoFrame{}; }
bool NDIDevice::startPlayout(const VideoMode& mode) { return false; }
bool NDIDevice::stopPlayout() { return false; }
bool NDIDevice::sendFrame(const VideoFrame& frame) { return false; }
void NDIDevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode NDIDevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> NDIDevice::discoverSources() {
    // TODO: Use NDI finder API to discover network sources
    return {};
}

void NDIDevice::setSourceName(const std::string& name) {
    impl_->sourceName = name;
}
