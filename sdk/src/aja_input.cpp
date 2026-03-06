/// @file aja_input.cpp
/// @brief AJA capture-only implementation.

#include "visioncast_sdk/aja_input.h"

#include <iostream>

struct AJAInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "AJA Input";
    int channel = 0;
};

AJAInput::AJAInput() : impl_(std::make_unique<Impl>()) {}
AJAInput::~AJAInput() { close(); }

bool AJAInput::open(const DeviceConfig& config) {
#ifdef HAS_AJA
    // TODO: Initialize AJA NTV2 SDK, open capture channel
    impl_->name = config.name.empty() ? "AJA Input" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[AJAInput] SDK not available." << std::endl;
    return false;
#endif
}

void AJAInput::close() {
    impl_->capturing = false;
    impl_->isOpen = false;
}

bool AJAInput::isOpen() const { return impl_->isOpen; }
std::string AJAInput::deviceName() const { return impl_->name; }
DeviceType AJAInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> AJAInput::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::UYVY, false},
        {1920, 1080, 29.97,  PixelFormat::UYVY, false},
        {1920, 1080, 50.0,   PixelFormat::UYVY, false},
        {1920, 1080, 59.94,  PixelFormat::UYVY, false},
        {3840, 2160, 25.0,   PixelFormat::V210, false},
        {3840, 2160, 29.97,  PixelFormat::V210, false},
        {3840, 2160, 50.0,   PixelFormat::V210, false},
        {3840, 2160, 59.94,  PixelFormat::V210, false},
    };
}

bool AJAInput::startCapture(const VideoMode& mode) {
#ifdef HAS_AJA
    // TODO: Configure AJA capture with selected mode & channel
    impl_->currentMode = mode;
    impl_->capturing = true;
    return true;
#else
    return false;
#endif
}

bool AJAInput::stopCapture() {
    impl_->capturing = false;
    return true;
}

VideoFrame AJAInput::captureFrame() {
    // TODO: Retrieve captured frame from AJA DMA ring buffer
    return VideoFrame{};
}

// Playout no-ops
bool AJAInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool AJAInput::stopPlayout() { return false; }
bool AJAInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void AJAInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode AJAInput::currentMode() const { return impl_->currentMode; }

// AJAInput-specific
void AJAInput::setChannel(int channel) {
    impl_->channel = channel;
}

int AJAInput::channel() const {
    return impl_->channel;
}

bool AJAInput::hasSignal() const {
    // TODO: Query AJA NTV2 SDK for signal presence
    return false;
}

VideoMode AJAInput::detectedMode() const {
    // TODO: Query AJA NTV2 SDK for detected input format
    return VideoMode{};
}

std::vector<DeviceConfig> AJAInput::enumerateDevices() {
    // TODO: Use AJA NTV2 API to discover capture devices
    return {};
}
