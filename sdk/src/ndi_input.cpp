/// @file ndi_input.cpp
/// @brief NDI receive-only implementation.

#include "visioncast_sdk/ndi_input.h"

#include <iostream>

struct NDIInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "NDI Input";
};

NDIInput::NDIInput() : impl_(std::make_unique<Impl>()) {}
NDIInput::~NDIInput() { close(); }

bool NDIInput::open(const DeviceConfig& config) {
#ifdef HAS_NDI
    // TODO: Initialize NDI SDK, create receiver for the named source
    impl_->name = config.name.empty() ? "NDI Input" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[NDIInput] SDK not available." << std::endl;
    return false;
#endif
}

void NDIInput::close() {
    impl_->capturing = false;
    impl_->isOpen = false;
}

bool NDIInput::isOpen() const { return impl_->isOpen; }
std::string NDIInput::deviceName() const { return impl_->name; }
DeviceType NDIInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> NDIInput::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::UYVY, false},
        {1920, 1080, 29.97,  PixelFormat::UYVY, false},
        {1920, 1080, 50.0,   PixelFormat::UYVY, false},
        {1920, 1080, 59.94,  PixelFormat::UYVY, false},
        {3840, 2160, 25.0,   PixelFormat::UYVY, false},
        {3840, 2160, 29.97,  PixelFormat::UYVY, false},
        {3840, 2160, 50.0,   PixelFormat::UYVY, false},
        {3840, 2160, 59.94,  PixelFormat::UYVY, false},
    };
}

bool NDIInput::startCapture(const VideoMode& mode) {
#ifdef HAS_NDI
    // TODO: Connect NDI receiver to source and start capturing
    impl_->currentMode = mode;
    impl_->capturing = true;
    return true;
#else
    return false;
#endif
}

bool NDIInput::stopCapture() {
    impl_->capturing = false;
    return true;
}

VideoFrame NDIInput::captureFrame() {
    // TODO: Retrieve received frame from NDI receiver queue
    return VideoFrame{};
}

// Playout no-ops
bool NDIInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool NDIInput::stopPlayout() { return false; }
bool NDIInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void NDIInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode NDIInput::currentMode() const { return impl_->currentMode; }

// NDIInput-specific
std::vector<DeviceConfig> NDIInput::discoverSources() {
    // TODO: Use NDI finder API to discover network sources
    return {};
}

bool NDIInput::hasSignal() const {
    // TODO: Check NDI receiver connection status
    return false;
}

VideoMode NDIInput::detectedMode() const {
    // TODO: Query NDI receiver for stream format
    return VideoMode{};
}
