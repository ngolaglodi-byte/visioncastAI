/// @file magewell_input.cpp
/// @brief Magewell capture-only implementation.

#include "visioncast_sdk/magewell_input.h"

#include <iostream>

struct MagewellInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "Magewell Input";
};

MagewellInput::MagewellInput() : impl_(std::make_unique<Impl>()) {}
MagewellInput::~MagewellInput() { close(); }

bool MagewellInput::open(const DeviceConfig& config) {
#ifdef HAS_MAGEWELL
    // TODO: Initialize Magewell SDK, open capture device by index
    impl_->name = config.name.empty() ? "Magewell Input" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[MagewellInput] SDK not available." << std::endl;
    return false;
#endif
}

void MagewellInput::close() {
    impl_->capturing = false;
    impl_->isOpen = false;
}

bool MagewellInput::isOpen() const { return impl_->isOpen; }
std::string MagewellInput::deviceName() const { return impl_->name; }
DeviceType MagewellInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> MagewellInput::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::UYVY, false},
        {1920, 1080, 29.97,  PixelFormat::UYVY, false},
        {1920, 1080, 50.0,   PixelFormat::UYVY, false},
        {1920, 1080, 59.94,  PixelFormat::UYVY, false},
        {3840, 2160, 25.0,   PixelFormat::UYVY, false},
        {3840, 2160, 29.97,  PixelFormat::UYVY, false},
    };
}

bool MagewellInput::startCapture(const VideoMode& mode) {
#ifdef HAS_MAGEWELL
    // TODO: Configure Magewell capture with selected mode
    impl_->currentMode = mode;
    impl_->capturing = true;
    return true;
#else
    return false;
#endif
}

bool MagewellInput::stopCapture() {
    impl_->capturing = false;
    return true;
}

VideoFrame MagewellInput::captureFrame() {
    // TODO: Retrieve captured frame from Magewell ring buffer
    return VideoFrame{};
}

// Playout no-ops
bool MagewellInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool MagewellInput::stopPlayout() { return false; }
bool MagewellInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void MagewellInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode MagewellInput::currentMode() const { return impl_->currentMode; }

// MagewellInput-specific
bool MagewellInput::hasSignal() const {
    // TODO: Query Magewell SDK for signal presence
    return false;
}

VideoMode MagewellInput::detectedMode() const {
    // TODO: Query Magewell SDK for detected input format
    return VideoMode{};
}

std::vector<DeviceConfig> MagewellInput::enumerateDevices() {
    // TODO: Use Magewell API to discover capture devices
    return {};
}
