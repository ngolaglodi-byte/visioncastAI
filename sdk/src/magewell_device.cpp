/// @file magewell_device.cpp
/// @brief Magewell capture device implementation.

#include "visioncast_sdk/magewell_device.h"

#include <iostream>

struct MagewellDevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "Magewell";
};

MagewellDevice::MagewellDevice() : impl_(std::make_unique<Impl>()) {}
MagewellDevice::~MagewellDevice() { close(); }

bool MagewellDevice::open(const DeviceConfig& config) {
#ifdef HAS_MAGEWELL
    // TODO: Initialize Magewell SDK, open device by index
    impl_->name = config.name.empty() ? "Magewell" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[Magewell] SDK not available." << std::endl;
    return false;
#endif
}

void MagewellDevice::close() {
    impl_->isOpen = false;
}

bool MagewellDevice::isOpen() const { return impl_->isOpen; }
std::string MagewellDevice::deviceName() const { return impl_->name; }
DeviceType MagewellDevice::deviceType() const { return DeviceType::CAPTURE; }
std::vector<VideoMode> MagewellDevice::supportedModes() const { return {}; }
bool MagewellDevice::startCapture(const VideoMode& mode) { return false; }
bool MagewellDevice::stopCapture() { return false; }
VideoFrame MagewellDevice::captureFrame() { return VideoFrame{}; }
bool MagewellDevice::startPlayout(const VideoMode& mode) { return false; }
bool MagewellDevice::stopPlayout() { return false; }
bool MagewellDevice::sendFrame(const VideoFrame& frame) { return false; }
void MagewellDevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode MagewellDevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> MagewellDevice::enumerateDevices() {
    // TODO: Use Magewell API to discover devices
    return {};
}
