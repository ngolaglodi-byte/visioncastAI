/// @file srt_output.cpp
/// @brief SRT send-only implementation.

#include "visioncast_sdk/srt_output.h"

#include <iostream>

struct SRTOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "SRT Output";
    std::string destination = "srt://localhost:9000";
    int latencyMs = 120;
};

SRTOutput::SRTOutput() : impl_(std::make_unique<Impl>()) {}
SRTOutput::~SRTOutput() { close(); }

bool SRTOutput::open(const DeviceConfig& config) {
#ifdef HAS_SRT
    // TODO: Initialize SRT socket and connect to destination
    impl_->name = config.name.empty() ? "SRT Output" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[SRTOutput] SRT library not available." << std::endl;
    return false;
#endif
}

void SRTOutput::close() {
    impl_->playing = false;
    impl_->isOpen = false;
}

bool SRTOutput::isOpen() const { return impl_->isOpen; }
std::string SRTOutput::deviceName() const { return impl_->name; }
DeviceType SRTOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> SRTOutput::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::NV12, false},
        {1920, 1080, 29.97,  PixelFormat::NV12, false},
        {1920, 1080, 50.0,   PixelFormat::NV12, false},
        {1920, 1080, 59.94,  PixelFormat::NV12, false},
        {3840, 2160, 25.0,   PixelFormat::NV12, false},
        {3840, 2160, 29.97,  PixelFormat::NV12, false},
        {3840, 2160, 50.0,   PixelFormat::NV12, false},
        {3840, 2160, 59.94,  PixelFormat::NV12, false},
    };
}

// Capture no-ops
bool SRTOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool SRTOutput::stopCapture() { return false; }
VideoFrame SRTOutput::captureFrame() { return VideoFrame{}; }

bool SRTOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_SRT
    // TODO: Start SRT sender with selected video mode and encoder
    impl_->currentMode = mode;
    impl_->playing = true;
    return true;
#else
    return false;
#endif
}

bool SRTOutput::stopPlayout() {
    impl_->playing = false;
    return true;
}

bool SRTOutput::sendFrame(const VideoFrame& frame) {
    // TODO: Encode and send frame via SRT socket
    (void)frame;
    return impl_->playing;
}

void SRTOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode SRTOutput::currentMode() const { return impl_->currentMode; }

// SRTOutput-specific
void SRTOutput::setDestination(const std::string& url) {
    impl_->destination = url;
}

std::string SRTOutput::destination() const {
    return impl_->destination;
}

void SRTOutput::setLatency(int ms) {
    impl_->latencyMs = ms;
}

int SRTOutput::latency() const {
    return impl_->latencyMs;
}

bool SRTOutput::isOutputActive() const {
    return impl_->playing;
}
