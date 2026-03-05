/// @file aja_device.cpp
/// @brief AJA video device implementation.

#include "visioncast_sdk/aja_device.h"

#include <iostream>

struct AJADevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "AJA";
};

AJADevice::AJADevice() : impl_(std::make_unique<Impl>()) {}
AJADevice::~AJADevice() { close(); }

bool AJADevice::open(const DeviceConfig& config) {
#ifdef HAS_AJA
    // TODO: Initialize AJA NTV2 SDK, open device by index
    impl_->name = config.name.empty() ? "AJA" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[AJA] SDK not available." << std::endl;
    return false;
#endif
}

void AJADevice::close() {
    impl_->isOpen = false;
}

bool AJADevice::isOpen() const { return impl_->isOpen; }
std::string AJADevice::deviceName() const { return impl_->name; }
DeviceType AJADevice::deviceType() const { return DeviceType::BIDIRECTIONAL; }
std::vector<VideoMode> AJADevice::supportedModes() const { return {}; }
bool AJADevice::startCapture(const VideoMode& mode) { return false; }
bool AJADevice::stopCapture() { return false; }
VideoFrame AJADevice::captureFrame() { return VideoFrame{}; }
bool AJADevice::startPlayout(const VideoMode& mode) { return false; }
bool AJADevice::stopPlayout() { return false; }
bool AJADevice::sendFrame(const VideoFrame& frame) { return false; }
void AJADevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode AJADevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> AJADevice::enumerateDevices() {
    // TODO: Use AJA NTV2 API to discover devices
    return {};
}
