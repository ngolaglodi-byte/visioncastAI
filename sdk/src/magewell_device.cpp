/// @file magewell_device.cpp
/// @brief Magewell capture device implementation.

#include "visioncast_sdk/magewell_device.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_MAGEWELL
#include "LibMWCapture/MWCapture.h"
#endif

#include <mutex>

static const char* TAG = "MagewellDevice";

struct MagewellDevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "Magewell";
    std::mutex mutex;

#ifdef HAS_MAGEWELL
    HCHANNEL channel = nullptr;
#endif
};

MagewellDevice::MagewellDevice() : impl_(std::make_unique<Impl>()) {}
MagewellDevice::~MagewellDevice() { close(); }

bool MagewellDevice::open(const DeviceConfig& config) {
#ifdef HAS_MAGEWELL
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening Magewell device index=" +
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
        impl_->name = config.name.empty() ? "Magewell" : config.name;
    }
    impl_->isOpen = true;
    SDKLogger::info(TAG, "Device opened: " + impl_->name);
    return true;
#else
    SDKLogger::warn(TAG, "Magewell SDK not available — device will not open");
    return false;
#endif
}

void MagewellDevice::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing Magewell device: " + impl_->name);
#ifdef HAS_MAGEWELL
    if (impl_->channel) {
        MWCloseChannel(impl_->channel);
        impl_->channel = nullptr;
    }
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Device closed");
}

bool MagewellDevice::isOpen() const { return impl_->isOpen; }
std::string MagewellDevice::deviceName() const { return impl_->name; }
DeviceType MagewellDevice::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> MagewellDevice::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::UYVY, false},
        {1920, 1080, 29.97,  PixelFormat::UYVY, false},
        {1920, 1080, 50.0,   PixelFormat::UYVY, false},
        {1920, 1080, 59.94,  PixelFormat::UYVY, false},
        {3840, 2160, 25.0,   PixelFormat::UYVY, false},
        {3840, 2160, 29.97,  PixelFormat::UYVY, false},
    };
}

bool MagewellDevice::startCapture(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startCapture() — use MagewellInput for capture");
    return false;
}
bool MagewellDevice::stopCapture() { return false; }
VideoFrame MagewellDevice::captureFrame() { return VideoFrame{}; }
bool MagewellDevice::startPlayout(const VideoMode& /*mode*/) { return false; }
bool MagewellDevice::stopPlayout() { return false; }
bool MagewellDevice::sendFrame(const VideoFrame& /*frame*/) { return false; }
void MagewellDevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode MagewellDevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> MagewellDevice::enumerateDevices() {
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

