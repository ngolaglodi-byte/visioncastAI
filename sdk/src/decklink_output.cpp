/// @file decklink_output.cpp
/// @brief Blackmagic DeckLink playout-only implementation.

#include "visioncast_sdk/decklink_output.h"

#include <iostream>

struct DeckLinkOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "DeckLink Output";
    DeckLinkConnector connector = DeckLinkConnector::AUTO;
    DeckLinkReference reference = DeckLinkReference::FREE_RUN;
};

DeckLinkOutput::DeckLinkOutput() : impl_(std::make_unique<Impl>()) {}
DeckLinkOutput::~DeckLinkOutput() { close(); }

bool DeckLinkOutput::open(const DeviceConfig& config) {
#ifdef HAS_DECKLINK
    // TODO: Initialize DeckLink SDK, open playout device by index
    impl_->name = config.name.empty() ? "DeckLink Output" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[DeckLinkOutput] SDK not available." << std::endl;
    return false;
#endif
}

void DeckLinkOutput::close() {
    impl_->playing = false;
    impl_->isOpen = false;
}

bool DeckLinkOutput::isOpen() const { return impl_->isOpen; }
std::string DeckLinkOutput::deviceName() const { return impl_->name; }
DeviceType DeckLinkOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> DeckLinkOutput::supportedModes() const {
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

// Capture no-ops
bool DeckLinkOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool DeckLinkOutput::stopCapture() { return false; }
VideoFrame DeckLinkOutput::captureFrame() { return VideoFrame{}; }

bool DeckLinkOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_DECKLINK
    // TODO: Configure DeckLink output with selected mode & connector
    impl_->currentMode = mode;
    impl_->playing = true;
    return true;
#else
    return false;
#endif
}

bool DeckLinkOutput::stopPlayout() {
    impl_->playing = false;
    return true;
}

bool DeckLinkOutput::sendFrame(const VideoFrame& frame) {
    // TODO: Schedule frame for DeckLink output
    (void)frame;
    return impl_->playing;
}

void DeckLinkOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode DeckLinkOutput::currentMode() const { return impl_->currentMode; }

// DeckLinkOutput-specific
void DeckLinkOutput::setConnector(DeckLinkConnector connector) {
    impl_->connector = connector;
}

DeckLinkConnector DeckLinkOutput::connector() const {
    return impl_->connector;
}

void DeckLinkOutput::setReferenceSource(DeckLinkReference ref) {
    impl_->reference = ref;
}

DeckLinkReference DeckLinkOutput::referenceSource() const {
    return impl_->reference;
}

bool DeckLinkOutput::isOutputActive() const {
    return impl_->playing;
}

std::vector<DeviceConfig> DeckLinkOutput::enumerateDevices() {
    // TODO: Use DeckLink API iterator to discover playout devices
    return {};
}
