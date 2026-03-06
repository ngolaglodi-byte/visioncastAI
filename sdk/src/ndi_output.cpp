/// @file ndi_output.cpp
/// @brief NDI send-only implementation.

#include "visioncast_sdk/ndi_output.h"

#include <iostream>

struct NDIOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "NDI Output";
    std::string sourceName = "VisionCast";
};

NDIOutput::NDIOutput() : impl_(std::make_unique<Impl>()) {}
NDIOutput::~NDIOutput() { close(); }

bool NDIOutput::open(const DeviceConfig& config) {
#ifdef HAS_NDI
    // TODO: Initialize NDI SDK, create sender with source name
    impl_->name = config.name.empty() ? "NDI Output" : config.name;
    impl_->isOpen = true;
    return true;
#else
    std::cerr << "[NDIOutput] SDK not available." << std::endl;
    return false;
#endif
}

void NDIOutput::close() {
    impl_->playing = false;
    impl_->isOpen = false;
}

bool NDIOutput::isOpen() const { return impl_->isOpen; }
std::string NDIOutput::deviceName() const { return impl_->name; }
DeviceType NDIOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> NDIOutput::supportedModes() const {
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

// Capture no-ops
bool NDIOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool NDIOutput::stopCapture() { return false; }
VideoFrame NDIOutput::captureFrame() { return VideoFrame{}; }

bool NDIOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_NDI
    // TODO: Start NDI sender with selected video mode
    impl_->currentMode = mode;
    impl_->playing = true;
    return true;
#else
    return false;
#endif
}

bool NDIOutput::stopPlayout() {
    impl_->playing = false;
    return true;
}

bool NDIOutput::sendFrame(const VideoFrame& frame) {
    // TODO: Send frame via NDI sender
    (void)frame;
    return impl_->playing;
}

void NDIOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode NDIOutput::currentMode() const { return impl_->currentMode; }

// NDIOutput-specific
void NDIOutput::setSourceName(const std::string& name) {
    impl_->sourceName = name;
}

std::string NDIOutput::sourceName() const {
    return impl_->sourceName;
}

bool NDIOutput::isOutputActive() const {
    return impl_->playing;
}
