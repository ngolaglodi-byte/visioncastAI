/// @file aja_output.cpp
/// @brief AJA playout-only implementation.

#include "visioncast_sdk/aja_output.h"

#include <iostream>

struct AJAOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "AJA Output";
    int channel = 0;
    AJAReference reference = AJAReference::FREE_RUN;
};

AJAOutput::AJAOutput() : impl_(std::make_unique<Impl>()) {}
AJAOutput::~AJAOutput() { close(); }

bool AJAOutput::open(const DeviceConfig& config) {
#ifdef HAS_AJA
    // TODO: Initialize AJA NTV2 SDK, open playout channel
    impl_->name = config.name.empty() ? "AJA Output" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[AJAOutput] SDK not available." << std::endl;
    return false;
#endif
}

void AJAOutput::close() {
    impl_->playing = false;
    impl_->isOpen = false;
}

bool AJAOutput::isOpen() const { return impl_->isOpen; }
std::string AJAOutput::deviceName() const { return impl_->name; }
DeviceType AJAOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> AJAOutput::supportedModes() const {
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
bool AJAOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool AJAOutput::stopCapture() { return false; }
VideoFrame AJAOutput::captureFrame() { return VideoFrame{}; }

bool AJAOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_AJA
    // TODO: Configure AJA output with selected mode & channel
    impl_->currentMode = mode;
    impl_->playing = true;
    return true;
#else
    return false;
#endif
}

bool AJAOutput::stopPlayout() {
    impl_->playing = false;
    return true;
}

bool AJAOutput::sendFrame(const VideoFrame& frame) {
    // TODO: Schedule frame for AJA DMA output
    (void)frame;
    return impl_->playing;
}

void AJAOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode AJAOutput::currentMode() const { return impl_->currentMode; }

// AJAOutput-specific
void AJAOutput::setChannel(int channel) {
    impl_->channel = channel;
}

int AJAOutput::channel() const {
    return impl_->channel;
}

void AJAOutput::setReferenceSource(AJAReference ref) {
    impl_->reference = ref;
}

AJAReference AJAOutput::referenceSource() const {
    return impl_->reference;
}

bool AJAOutput::isOutputActive() const {
    return impl_->playing;
}

std::vector<DeviceConfig> AJAOutput::enumerateDevices() {
    // TODO: Use AJA NTV2 API to discover playout devices
    return {};
}
