/// @file decklink_device.cpp
/// @brief Blackmagic DeckLink device implementation.

#include "visioncast_sdk/decklink_device.h"

#include <iostream>

struct DeckLinkDevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "DeckLink";
};

DeckLinkDevice::DeckLinkDevice() : impl_(std::make_unique<Impl>()) {}
DeckLinkDevice::~DeckLinkDevice() { close(); }

bool DeckLinkDevice::open(const DeviceConfig& config) {
#ifdef HAS_DECKLINK
    // TODO: Initialize DeckLink SDK, open device by index
    impl_->name = config.name.empty() ? "DeckLink" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[DeckLink] SDK not available." << std::endl;
    return false;
#endif
}

void DeckLinkDevice::close() {
    impl_->isOpen = false;
}

bool DeckLinkDevice::isOpen() const { return impl_->isOpen; }
std::string DeckLinkDevice::deviceName() const { return impl_->name; }
DeviceType DeckLinkDevice::deviceType() const { return DeviceType::BIDIRECTIONAL; }
std::vector<VideoMode> DeckLinkDevice::supportedModes() const { return {}; }
bool DeckLinkDevice::startCapture(const VideoMode& mode) { return false; }
bool DeckLinkDevice::stopCapture() { return false; }
VideoFrame DeckLinkDevice::captureFrame() { return VideoFrame{}; }
bool DeckLinkDevice::startPlayout(const VideoMode& mode) { return false; }
bool DeckLinkDevice::stopPlayout() { return false; }
bool DeckLinkDevice::sendFrame(const VideoFrame& frame) { return false; }
void DeckLinkDevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode DeckLinkDevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> DeckLinkDevice::enumerateDevices() {
    // TODO: Use DeckLink API iterator to discover devices
    return {};
}
