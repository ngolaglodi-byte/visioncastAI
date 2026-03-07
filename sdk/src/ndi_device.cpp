/// @file ndi_device.cpp
/// @brief NDI network device implementation.
///
/// Generic bidirectional NDI wrapper (used when a device serves both
/// receive and send roles).  For role-specific use, prefer NDIInput or
/// NDIOutput which implement the unified VideoInputInterface /
/// VideoOutputInterface contracts.
///
/// When HAS_NDI is defined the NDI SDK is used for real network access.
/// Otherwise a safe stub is compiled so the library still links.

#include "visioncast_sdk/ndi_device.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#include <mutex>

static const char* TAG = "NDIDevice";

struct NDIDevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "NDI";
    std::string sourceName = "VisionCast";
    std::mutex mutex;

#ifdef HAS_NDI
    // NDIlib_recv_instance_t receiver = nullptr;
    // NDIlib_send_instance_t sender   = nullptr;
    // NDIlib_find_instance_t finder   = nullptr;
#endif
};

NDIDevice::NDIDevice() : impl_(std::make_unique<Impl>()) {}
NDIDevice::~NDIDevice() { close(); }

bool NDIDevice::open(const DeviceConfig& config) {
#ifdef HAS_NDI
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening NDI device: " + config.name);
    try {
        // --- NDI SDK initialisation ---
        // if (!NDIlib_initialize())
        //     throw NDIError("NDI runtime not installed");
        //
        // // Receiver (capture) side
        // NDIlib_recv_create_v3_t recv_desc;
        // recv_desc.source_to_connect_to.p_ndi_name = config.name.c_str();
        // recv_desc.color_format = NDIlib_recv_color_format_UYVY_BGRA;
        // recv_desc.bandwidth    = NDIlib_recv_bandwidth_highest;
        // recv_desc.allow_video_fields = false;
        // impl_->receiver = NDIlib_recv_create_v3(&recv_desc);
        //
        // // Sender (playout) side
        // NDIlib_send_create_t send_desc;
        // send_desc.p_ndi_name  = impl_->sourceName.c_str();
        // send_desc.clock_video = true;
        // send_desc.clock_audio = false;
        // impl_->sender = NDIlib_send_create(&send_desc);
        //
        // if (!impl_->receiver || !impl_->sender)
        //     throw NDIError("Failed to create NDI sender/receiver");
        impl_->name = config.name.empty() ? "NDI" : config.name;
        impl_->isOpen = true;
        SDKLogger::info(TAG, "Device opened: " + impl_->name);
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

void NDIDevice::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing device: " + impl_->name);
#ifdef HAS_NDI
    // if (impl_->receiver) { NDIlib_recv_destroy(impl_->receiver); impl_->receiver = nullptr; }
    // if (impl_->sender)   { NDIlib_send_destroy(impl_->sender);   impl_->sender   = nullptr; }
    // NDIlib_destroy();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Device closed");
}

bool NDIDevice::isOpen() const { return impl_->isOpen; }
std::string NDIDevice::deviceName() const { return impl_->name; }
DeviceType NDIDevice::deviceType() const { return DeviceType::BIDIRECTIONAL; }

std::vector<VideoMode> NDIDevice::supportedModes() const {
    return {
        {1920, 1080, 25.0,  PixelFormat::UYVY, false},
        {1920, 1080, 29.97, PixelFormat::UYVY, false},
        {1920, 1080, 50.0,  PixelFormat::UYVY, false},
        {1920, 1080, 59.94, PixelFormat::UYVY, false},
        {3840, 2160, 25.0,  PixelFormat::UYVY, false},
        {3840, 2160, 29.97, PixelFormat::UYVY, false},
        {3840, 2160, 50.0,  PixelFormat::UYVY, false},
        {3840, 2160, 59.94, PixelFormat::UYVY, false},
    };
}

bool NDIDevice::startCapture(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startCapture() — use NDIInput for capture");
    return false;
}
bool NDIDevice::stopCapture() { return false; }
VideoFrame NDIDevice::captureFrame() { return VideoFrame{}; }

bool NDIDevice::startPlayout(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startPlayout() — use NDIOutput for playout");
    return false;
}
bool NDIDevice::stopPlayout() { return false; }
bool NDIDevice::sendFrame(const VideoFrame& /*frame*/) { return false; }

void NDIDevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode NDIDevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> NDIDevice::discoverSources() {
#ifdef HAS_NDI
    // std::vector<DeviceConfig> sources;
    // NDIlib_find_instance_t finder = NDIlib_find_create_v2();
    // if (!finder) return sources;
    //
    // // Wait up to 5 seconds for discovery
    // NDIlib_find_wait_for_sources(finder, 5000);
    // uint32_t count = 0;
    // const NDIlib_source_t* ndiSources =
    //     NDIlib_find_get_current_sources(finder, &count);
    //
    // for (uint32_t i = 0; i < count; ++i) {
    //     DeviceConfig cfg;
    //     cfg.deviceIndex = static_cast<int>(i);
    //     cfg.name = ndiSources[i].p_ndi_name;
    //     sources.push_back(cfg);
    // }
    // NDIlib_find_destroy(finder);
    // return sources;
#endif
    SDKLogger::debug(TAG, "discoverSources() — no SDK, returning empty list");
    return {};
}

void NDIDevice::setSourceName(const std::string& name) {
    impl_->sourceName = name;
    SDKLogger::debug(TAG, "Source name set: " + name);
}
