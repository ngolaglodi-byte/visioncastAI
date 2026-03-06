/// @file decklink_input.cpp
/// @brief Blackmagic DeckLink capture-only implementation.

#include "visioncast_sdk/decklink_input.h"

#include <iostream>

struct DeckLinkInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "DeckLink Input";
    DeckLinkConnector connector = DeckLinkConnector::AUTO;
};

DeckLinkInput::DeckLinkInput() : impl_(std::make_unique<Impl>()) {}
DeckLinkInput::~DeckLinkInput() { close(); }

bool DeckLinkInput::open(const DeviceConfig& config) {
#ifdef HAS_DECKLINK
    // TODO: Initialize DeckLink SDK, open capture device by index
    impl_->name = config.name.empty() ? "DeckLink Input" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[DeckLinkInput] SDK not available." << std::endl;
    return false;
#endif
}

void DeckLinkInput::close() {
    impl_->capturing = false;
    impl_->isOpen = false;
}

bool DeckLinkInput::isOpen() const { return impl_->isOpen; }
std::string DeckLinkInput::deviceName() const { return impl_->name; }
DeviceType DeckLinkInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> DeckLinkInput::supportedModes() const {
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

bool DeckLinkInput::startCapture(const VideoMode& mode) {
#ifdef HAS_DECKLINK
    // TODO: Configure DeckLink capture with selected mode & connector
    impl_->currentMode = mode;
    impl_->capturing = true;
    return true;
#else
    return false;
#endif
}

bool DeckLinkInput::stopCapture() {
    impl_->capturing = false;
    return true;
}

VideoFrame DeckLinkInput::captureFrame() {
    // TODO: Retrieve captured frame from DeckLink callback queue
    return VideoFrame{};
}

// Playout no-ops
bool DeckLinkInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool DeckLinkInput::stopPlayout() { return false; }
bool DeckLinkInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void DeckLinkInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode DeckLinkInput::currentMode() const { return impl_->currentMode; }

// DeckLinkInput-specific
void DeckLinkInput::setConnector(DeckLinkConnector connector) {
    impl_->connector = connector;
}

DeckLinkConnector DeckLinkInput::connector() const {
    return impl_->connector;
}

bool DeckLinkInput::hasSignal() const {
    // TODO: Query DeckLink SDK for signal presence
    return false;
}

VideoMode DeckLinkInput::detectedMode() const {
    // TODO: Query DeckLink SDK for detected input format
    return VideoMode{};
}

std::vector<DeviceConfig> DeckLinkInput::enumerateDevices() {
    // TODO: Use DeckLink API iterator to discover capture devices
    return {};
}
