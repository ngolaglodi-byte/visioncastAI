/// @file rtmp_output.cpp
/// @brief RTMP send-only implementation.

#include "visioncast_sdk/rtmp_output.h"

#include <iostream>

struct RTMPOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "RTMP Output";
    std::string serverUrl;
    std::string streamKey;
};

RTMPOutput::RTMPOutput() : impl_(std::make_unique<Impl>()) {}
RTMPOutput::~RTMPOutput() { close(); }

bool RTMPOutput::open(const DeviceConfig& config) {
#ifdef HAS_RTMP
    // TODO: Initialize RTMP connection via FFmpeg/librtmp
    impl_->name = config.name.empty() ? "RTMP Output" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[RTMPOutput] RTMP library not available." << std::endl;
    return false;
#endif
}

void RTMPOutput::close() {
    impl_->playing = false;
    impl_->isOpen = false;
}

bool RTMPOutput::isOpen() const { return impl_->isOpen; }
std::string RTMPOutput::deviceName() const { return impl_->name; }
DeviceType RTMPOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> RTMPOutput::supportedModes() const {
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
bool RTMPOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool RTMPOutput::stopCapture() { return false; }
VideoFrame RTMPOutput::captureFrame() { return VideoFrame{}; }

bool RTMPOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_RTMP
    // TODO: Start RTMP stream with selected video mode and encoder
    impl_->currentMode = mode;
    impl_->playing = true;
    return true;
#else
    return false;
#endif
}

bool RTMPOutput::stopPlayout() {
    impl_->playing = false;
    return true;
}

bool RTMPOutput::sendFrame(const VideoFrame& frame) {
    // TODO: Encode and send frame via RTMP connection
    (void)frame;
    return impl_->playing;
}

void RTMPOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode RTMPOutput::currentMode() const { return impl_->currentMode; }

// RTMPOutput-specific
void RTMPOutput::setServerUrl(const std::string& url) {
    impl_->serverUrl = url;
}

std::string RTMPOutput::serverUrl() const {
    return impl_->serverUrl;
}

void RTMPOutput::setStreamKey(const std::string& key) {
    impl_->streamKey = key;
}

std::string RTMPOutput::streamKey() const {
    return impl_->streamKey;
}

bool RTMPOutput::isOutputActive() const {
    return impl_->playing;
}
