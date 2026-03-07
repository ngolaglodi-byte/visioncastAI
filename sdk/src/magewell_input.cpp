/// @file magewell_input.cpp
/// @brief Magewell capture-only implementation.

#include "visioncast_sdk/magewell_input.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_MAGEWELL
#include "LibMWCapture/MWCapture.h"
#include <windows.h>
#endif

#include <mutex>
#include <vector>

static const char* TAG = "MagewellInput";

struct MagewellInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "Magewell Input";
    std::mutex mutex;

    // Frame buffer owned by us; size = stride * height.
    std::vector<uint8_t> frameBuf;

#ifdef HAS_MAGEWELL
    HCHANNEL channel    = nullptr;
    HANDLE   notifyEvent = nullptr;
#endif
};

MagewellInput::MagewellInput() : impl_(std::make_unique<Impl>()) {}
MagewellInput::~MagewellInput() { close(); }

bool MagewellInput::open(const DeviceConfig& config) {
#ifdef HAS_MAGEWELL
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening Magewell capture device index=" +
                         std::to_string(config.deviceIndex));
    MWRefreshDevice();
    int count = MWGetChannelCount();
    if (config.deviceIndex >= count) {
        SDKLogger::error(TAG, "Magewell device not found at index " +
                              std::to_string(config.deviceIndex));
        return false;
    }
    impl_->channel = MWOpenChannelByIndex(config.deviceIndex);
    if (!impl_->channel) {
        SDKLogger::error(TAG, "MWOpenChannelByIndex failed");
        return false;
    }
    MWCAP_CHANNEL_INFO info;
    if (MWGetChannelInfo(impl_->channel, &info) == MW_SUCCEEDED) {
        impl_->name = config.name.empty()
                      ? std::string(info.szProductName)
                      : config.name;
    } else {
        impl_->name = config.name.empty() ? "Magewell Input" : config.name;
    }
    impl_->isOpen = true;
    SDKLogger::info(TAG, "Capture device opened: " + impl_->name);
    return true;
#else
    SDKLogger::warn(TAG, "Magewell SDK not available — device will not open");
    return false;
#endif
}

void MagewellInput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing Magewell capture device: " + impl_->name);
    if (impl_->capturing) {
#ifdef HAS_MAGEWELL
        MWStopVideoCapture(impl_->channel);
        if (impl_->notifyEvent) {
            CloseHandle(impl_->notifyEvent);
            impl_->notifyEvent = nullptr;
        }
#endif
        impl_->capturing = false;
    }
#ifdef HAS_MAGEWELL
    if (impl_->channel) {
        MWCloseChannel(impl_->channel);
        impl_->channel = nullptr;
    }
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Capture device closed");
}

bool MagewellInput::isOpen() const { return impl_->isOpen; }
std::string MagewellInput::deviceName() const { return impl_->name; }
DeviceType MagewellInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> MagewellInput::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::UYVY, false},
        {1920, 1080, 29.97,  PixelFormat::UYVY, false},
        {1920, 1080, 50.0,   PixelFormat::UYVY, false},
        {1920, 1080, 59.94,  PixelFormat::UYVY, false},
        {3840, 2160, 25.0,   PixelFormat::UYVY, false},
        {3840, 2160, 29.97,  PixelFormat::UYVY, false},
    };
}

bool MagewellInput::startCapture(const VideoMode& mode) {
#ifdef HAS_MAGEWELL
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startCapture() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting Magewell capture " +
                         std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));

    impl_->notifyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    MW_RESULT res = MWStartVideoCapture(impl_->channel, impl_->notifyEvent);
    if (res != MW_SUCCEEDED) {
        SDKLogger::error(TAG, "MWStartVideoCapture failed");
        CloseHandle(impl_->notifyEvent);
        impl_->notifyEvent = nullptr;
        return false;
    }
    const int stride = mode.width * 2; // UYVY: 2 bytes per pixel
    impl_->frameBuf.resize(static_cast<size_t>(stride) * mode.height);
    impl_->currentMode = mode;
    impl_->capturing = true;
    SDKLogger::info(TAG, "Magewell capture started");
    return true;
#else
    return false;
#endif
}

bool MagewellInput::stopCapture() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->capturing) return true;
    SDKLogger::info(TAG, "Stopping Magewell capture");
#ifdef HAS_MAGEWELL
    MWStopVideoCapture(impl_->channel);
    if (impl_->notifyEvent) {
        CloseHandle(impl_->notifyEvent);
        impl_->notifyEvent = nullptr;
    }
#endif
    impl_->capturing = false;
    return true;
}

VideoFrame MagewellInput::captureFrame() {
#ifdef HAS_MAGEWELL
    if (!impl_->capturing || impl_->frameBuf.empty()) return VideoFrame{};

    const int width  = impl_->currentMode.width;
    const int height = impl_->currentMode.height;
    const int stride = width * 2; // UYVY

    // Wait for a new frame notification (up to 200 ms)
    if (impl_->notifyEvent)
        WaitForSingleObject(impl_->notifyEvent, 200);

    MW_RESULT res = MWCaptureVideoFrameToVirtualAddress(
        impl_->channel,
        -1 /*use current frame*/,
        impl_->frameBuf.data(),
        static_cast<DWORD>(impl_->frameBuf.size()),
        static_cast<DWORD>(stride),
        FALSE /*bottom-up*/,
        nullptr /*reserved*/,
        FOURCC_UYV2,
        width,
        height);

    if (res != MW_SUCCEEDED) {
        SDKLogger::debug(TAG, "captureFrame() — MWCaptureVideoFrameToVirtualAddress failed");
        return VideoFrame{};
    }

    VideoFrame frame;
    frame.data   = impl_->frameBuf.data();
    frame.width  = width;
    frame.height = height;
    frame.stride = stride;
    frame.format = PixelFormat::UYVY;
    return frame;
#endif
    SDKLogger::debug(TAG, "captureFrame() — no Magewell SDK, returning empty frame");
    return VideoFrame{};
}

// Playout no-ops
bool MagewellInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool MagewellInput::stopPlayout() { return false; }
bool MagewellInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void MagewellInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode MagewellInput::currentMode() const { return impl_->currentMode; }

// MagewellInput-specific
bool MagewellInput::hasSignal() const {
#ifdef HAS_MAGEWELL
    if (!impl_->channel) return false;
    MWCAP_VIDEO_SIGNAL_STATUS status;
    if (MWGetVideoSignalStatus(impl_->channel, &status) == MW_SUCCEEDED)
        return status.state == MWCAP_VIDEO_SIGNAL_LOCKED;
#endif
    return false;
}

VideoMode MagewellInput::detectedMode() const {
#ifdef HAS_MAGEWELL
    if (!impl_->channel) return VideoMode{};
    MWCAP_VIDEO_SIGNAL_STATUS status;
    if (MWGetVideoSignalStatus(impl_->channel, &status) != MW_SUCCEEDED)
        return VideoMode{};
    if (status.state != MWCAP_VIDEO_SIGNAL_LOCKED)
        return VideoMode{};
    // Derive frame rate from field rate (status.dwFrameDuration is in 100ns units)
    double fps = 0.0;
    if (status.dwFrameDuration > 0)
        fps = 1e7 / static_cast<double>(status.dwFrameDuration);
    return VideoMode{
        status.cx,
        status.cy,
        fps,
        PixelFormat::UYVY,
        status.bInterlaced != 0
    };
#endif
    return VideoMode{};
}

std::vector<DeviceConfig> MagewellInput::enumerateDevices() {
#ifdef HAS_MAGEWELL
    std::vector<DeviceConfig> devices;
    MWRefreshDevice();
    int count = MWGetChannelCount();
    for (int i = 0; i < count; ++i) {
        HCHANNEL ch = MWOpenChannelByIndex(i);
        if (!ch) continue;
        MWCAP_CHANNEL_INFO info;
        DeviceConfig cfg;
        cfg.deviceIndex = i;
        if (MWGetChannelInfo(ch, &info) == MW_SUCCEEDED)
            cfg.name = info.szProductName;
        else
            cfg.name = "Magewell";
        devices.push_back(cfg);
        MWCloseChannel(ch);
    }
    SDKLogger::info(TAG, "enumerateDevices() found " +
                         std::to_string(devices.size()) + " Magewell device(s)");
    return devices;
#endif
    SDKLogger::debug(TAG, "enumerateDevices() — no Magewell SDK, returning empty list");
    return {};
}

