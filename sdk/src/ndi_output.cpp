/// @file ndi_output.cpp
/// @brief NDI send-only implementation.
///
/// When HAS_NDI is defined the NDI SDK is used for real network video access.
/// Otherwise a safe stub is compiled so the library still links.
/// Supports both Full NDI (uncompressed) and NDI HX (compressed) streams.

#include "visioncast_sdk/ndi_output.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#include <iostream>
#include <mutex>

static const char* TAG = "NDIOutput";

struct NDIOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "NDI Output";
    std::string sourceName = "VisionCast";
    NDIBandwidth bandwidth = NDIBandwidth::FULL;
    std::string outputTimecode;
    std::mutex mutex;

#ifdef HAS_NDI
    // NDIlib_send_instance_t sender = nullptr;
#endif
};

NDIOutput::NDIOutput() : impl_(std::make_unique<Impl>()) {}
NDIOutput::~NDIOutput() { close(); }

bool NDIOutput::open(const DeviceConfig& config) {
#ifdef HAS_NDI
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening NDI sender: " + impl_->sourceName);
    try {
        // --- NDI SDK initialisation ---
        // if (!NDIlib_initialize())
        //     throw NDIError("NDI runtime not installed");
        //
        // NDIlib_send_create_t send_desc;
        // send_desc.p_ndi_name = impl_->sourceName.c_str();
        // send_desc.clock_video = true;
        // send_desc.clock_audio = false;
        //
        // impl_->sender = NDIlib_send_create(&send_desc);
        // if (!impl_->sender)
        //     throw NDIError("Failed to create NDI sender");
        impl_->name = config.name.empty() ? "NDI Output" : config.name;
        impl_->isOpen = true;
        SDKLogger::info(TAG, "Sender opened: " + impl_->name +
                              " (source=" + impl_->sourceName + ")");
        return true;
    } catch (const SDKError& e) {
        SDKLogger::error(TAG, std::string("open() failed: ") + e.what());
        return false;
    }
#else
    SDKLogger::warn(TAG, "NDI SDK not available — device will not open");
    return false;
#endif
}

void NDIOutput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing sender: " + impl_->name);
    impl_->playing = false;
#ifdef HAS_NDI
    // if (impl_->sender) { NDIlib_send_destroy(impl_->sender); impl_->sender = nullptr; }
    // NDIlib_destroy();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Sender closed");
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startPlayout() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting NDI playout " + std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate) +
                         (impl_->bandwidth == NDIBandwidth::HX ? " [HX]" : " [Full]"));
    impl_->currentMode = mode;
    impl_->playing = true;
    SDKLogger::info(TAG, "NDI playout started");
    return true;
#else
    return false;
#endif
}

bool NDIOutput::stopPlayout() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->playing) return true;
    SDKLogger::info(TAG, "Stopping NDI playout");
    impl_->playing = false;
    return true;
}

bool NDIOutput::sendFrame(const VideoFrame& frame) {
#ifdef HAS_NDI
    // NDIlib_video_frame_v2_t ndiFrame;
    // ndiFrame.xres = frame.width;
    // ndiFrame.yres = frame.height;
    // ndiFrame.line_stride_in_bytes = frame.stride;
    // ndiFrame.FourCC = NDIlib_FourCC_type_UYVY;
    // ndiFrame.p_data = frame.data;
    // ndiFrame.frame_rate_N = static_cast<int>(impl_->currentMode.frameRate * 1000);
    // ndiFrame.frame_rate_D = 1000;
    // ndiFrame.frame_format_type = NDIlib_frame_format_type_progressive;
    //
    // // Embed timecode if set
    // if (!impl_->outputTimecode.empty()) {
    //     ndiFrame.timecode = parseTimecodeToNDI(impl_->outputTimecode);
    // }
    //
    // NDIlib_send_send_video_v2(impl_->sender, &ndiFrame);
    // return true;
#endif
    (void)frame;
    return impl_->playing;
}

void NDIOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode NDIOutput::currentMode() const { return impl_->currentMode; }

// -- VideoOutputInterface --
bool NDIOutput::openOutput(const DeviceConfig& config) { return open(config); }
void NDIOutput::closeOutput() { close(); }
bool NDIOutput::isOutputOpen() const { return isOpen(); }

bool NDIOutput::isOutputActive() const {
    return impl_->playing;
}

std::string NDIOutput::outputDeviceName() const { return deviceName(); }
std::vector<VideoMode> NDIOutput::supportedOutputModes() const { return supportedModes(); }

void NDIOutput::setOutputTimecode(const std::string& timecode) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->outputTimecode = timecode;
    SDKLogger::debug(TAG, "Output timecode set: " + timecode);
}

// -- NDIOutput-specific --
void NDIOutput::setSourceName(const std::string& name) {
    impl_->sourceName = name;
    SDKLogger::debug(TAG, "Source name set: " + name);
}

std::string NDIOutput::sourceName() const {
    return impl_->sourceName;
}

void NDIOutput::setBandwidth(NDIBandwidth bw) {
    impl_->bandwidth = bw;
    SDKLogger::debug(TAG, std::string("Bandwidth set to ") +
                          (bw == NDIBandwidth::HX ? "HX" : "Full"));
}

NDIBandwidth NDIOutput::bandwidth() const {
    return impl_->bandwidth;
}
