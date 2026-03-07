/// @file ndi_input.cpp
/// @brief NDI receive-only implementation.
///
/// When HAS_NDI is defined the NDI SDK is used for real network video access.
/// Otherwise a safe stub is compiled so the library still links.
/// Supports both Full NDI (uncompressed) and NDI HX (compressed) streams.

#include "visioncast_sdk/ndi_input.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"
#include "ndi_helpers.h"

#ifdef HAS_NDI
#include <Processing.NDI.Lib.h>
#endif

#include <cstring>
#include <mutex>
#include <vector>

static const char* TAG = "NDIInput";

struct NDIInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "NDI Input";
    NDIBandwidth bandwidth = NDIBandwidth::FULL;
    FrameCallback frameCallback = nullptr;
    std::string lastTimecode;
    std::mutex mutex;

    // Internal frame buffer — holds a copy of the last received frame so
    // that the caller does not need to free NDI SDK memory directly.
    std::vector<uint8_t> frameBuf;
    int frameWidth  = 0;
    int frameHeight = 0;
    int frameStride = 0;

#ifdef HAS_NDI
    NDIlib_recv_instance_t receiver = nullptr;
    NDIlib_find_instance_t finder   = nullptr;
#endif
};

NDIInput::NDIInput() : impl_(std::make_unique<Impl>()) {}
NDIInput::~NDIInput() { close(); }

bool NDIInput::open(const DeviceConfig& config) {
#ifdef HAS_NDI
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening NDI receiver for source: " + config.name);
    try {
        // --- NDI SDK initialisation ---
        if (!NDIlib_initialize())
            throw NDIError("NDI runtime not installed");

        NDIlib_recv_create_v3_t recv_desc;
        recv_desc.source_to_connect_to.p_ndi_name = config.name.c_str();
        recv_desc.color_format = NDIlib_recv_color_format_UYVY_BGRA;
        recv_desc.bandwidth = (impl_->bandwidth == NDIBandwidth::HX)
            ? NDIlib_recv_bandwidth_lowest
            : NDIlib_recv_bandwidth_highest;
        recv_desc.allow_video_fields = false;

        impl_->receiver = NDIlib_recv_create_v3(&recv_desc);
        if (!impl_->receiver)
            throw NDIError("Failed to create NDI receiver");

        impl_->name = config.name.empty() ? "NDI Input" : config.name;
        impl_->isOpen = true;
        SDKLogger::info(TAG, "Receiver opened: " + impl_->name);
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

void NDIInput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing receiver: " + impl_->name);
    impl_->capturing = false;
#ifdef HAS_NDI
    if (impl_->finder)   { NDIlib_find_destroy(impl_->finder);   impl_->finder   = nullptr; }
    if (impl_->receiver) { NDIlib_recv_destroy(impl_->receiver); impl_->receiver = nullptr; }
    NDIlib_destroy();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Receiver closed");
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startCapture() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting NDI capture " + std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate) +
                         (impl_->bandwidth == NDIBandwidth::HX ? " [HX]" : " [Full]"));
    // NDI receiver starts receiving as soon as it is created and connected.
    // The capture mode sets up the expected format for frame conversion.
    impl_->currentMode = mode;
    impl_->capturing = true;
    SDKLogger::info(TAG, "NDI capture started");
    return true;
#else
    return false;
#endif
}

bool NDIInput::stopCapture() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->capturing) return true;
    SDKLogger::info(TAG, "Stopping NDI capture");
    impl_->capturing = false;
    return true;
}

VideoFrame NDIInput::captureFrame() {
#ifdef HAS_NDI
    if (!impl_->receiver) {
        SDKLogger::debug(TAG, "captureFrame() — receiver not initialised");
        return VideoFrame{};
    }
    NDIlib_video_frame_v2_t ndiFrame;
    NDIlib_frame_type_e type = NDIlib_recv_capture_v3(
        impl_->receiver, &ndiFrame, nullptr, nullptr, 5000 /*ms timeout*/);

    if (type != NDIlib_frame_type_video) {
        SDKLogger::debug(TAG, "captureFrame() — no video frame available");
        return VideoFrame{};
    }

    // Copy pixel data so the SDK buffer can be released immediately.
    const size_t dataSize =
        static_cast<size_t>(ndiFrame.line_stride_in_bytes) * ndiFrame.yres;
    impl_->frameBuf.resize(dataSize);
    if (ndiFrame.p_data)
        std::memcpy(impl_->frameBuf.data(), ndiFrame.p_data, dataSize);

    impl_->frameWidth  = ndiFrame.xres;
    impl_->frameHeight = ndiFrame.yres;
    impl_->frameStride = ndiFrame.line_stride_in_bytes;

    // Timecode extraction
    impl_->lastTimecode = parseNDITimecode(ndiFrame.timecode,
                                           impl_->currentMode.frameRate);

    // Release the SDK frame buffer now that we have a local copy.
    NDIlib_recv_free_video_v2(impl_->receiver, &ndiFrame);

    VideoFrame frame;
    frame.data      = impl_->frameBuf.data();
    frame.width     = impl_->frameWidth;
    frame.height    = impl_->frameHeight;
    frame.stride    = impl_->frameStride;
    frame.format    = impl_->currentMode.format;
    return frame;
#endif
    SDKLogger::debug(TAG, "captureFrame() — no SDK, returning empty frame");
    return VideoFrame{};
}

// Playout no-ops
bool NDIInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool NDIInput::stopPlayout() { return false; }
bool NDIInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void NDIInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode NDIInput::currentMode() const { return impl_->currentMode; }

// -- VideoInputInterface --
bool NDIInput::openInput(const DeviceConfig& config) { return open(config); }
void NDIInput::closeInput() { close(); }
bool NDIInput::isInputOpen() const { return isOpen(); }

void NDIInput::setFrameCallback(FrameCallback callback) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->frameCallback = std::move(callback);
    SDKLogger::debug(TAG, "Frame callback registered");
}

bool NDIInput::hasSignal() const {
#ifdef HAS_NDI
    if (!impl_->receiver) return false;
    // NDIlib_recv_get_no_connections() returns the number of up-stream senders.
    return NDIlib_recv_get_no_connections(impl_->receiver) > 0;
#endif
    return false;
}

VideoMode NDIInput::detectedMode() const {
#ifdef HAS_NDI
    // NDI reports resolution and frame rate per-frame.
    // Return the mode from the last successfully captured frame.
    if (impl_->frameWidth > 0 && impl_->frameHeight > 0) {
        return VideoMode{impl_->frameWidth, impl_->frameHeight,
                         impl_->currentMode.frameRate,
                         impl_->currentMode.format, false};
    }
#endif
    return VideoMode{};
}

std::string NDIInput::inputDeviceName() const { return deviceName(); }
std::vector<VideoMode> NDIInput::supportedInputModes() const { return supportedModes(); }

std::string NDIInput::lastTimecode() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->lastTimecode;
}

// -- NDIInput-specific --
std::vector<DeviceConfig> NDIInput::discoverSources() {
#ifdef HAS_NDI
    std::vector<DeviceConfig> sources;
    NDIlib_find_instance_t finder = NDIlib_find_create_v2();
    if (!finder) {
        SDKLogger::warn(TAG, "discoverSources() — failed to create NDI finder");
        return sources;
    }

    // Wait up to 5 s for discovery
    NDIlib_find_wait_for_sources(finder, 5000);
    uint32_t count = 0;
    const NDIlib_source_t* ndiSources =
        NDIlib_find_get_current_sources(finder, &count);

    for (uint32_t i = 0; i < count; ++i) {
        DeviceConfig cfg;
        cfg.deviceIndex = static_cast<int>(i);
        cfg.name = ndiSources[i].p_ndi_name ? ndiSources[i].p_ndi_name : "NDI Source";
        sources.push_back(cfg);
    }
    NDIlib_find_destroy(finder);
    SDKLogger::info(TAG, "discoverSources() found " +
                         std::to_string(sources.size()) + " NDI source(s)");
    return sources;
#endif
    SDKLogger::debug(TAG, "discoverSources() — no SDK, returning empty list");
    return {};
}

void NDIInput::setBandwidth(NDIBandwidth bw) {
    impl_->bandwidth = bw;
    SDKLogger::debug(TAG, std::string("Bandwidth set to ") +
                          (bw == NDIBandwidth::HX ? "HX" : "Full"));
}

NDIBandwidth NDIInput::bandwidth() const {
    return impl_->bandwidth;
}
