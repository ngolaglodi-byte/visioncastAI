/// @file aja_device.cpp
/// @brief AJA video device implementation.

#include "visioncast_sdk/aja_device.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_AJA
#include "ntv2device.h"
#include "ntv2devicescanner.h"
#include "ntv2utils.h"
#endif

#include <mutex>

static const char* TAG = "AJADevice";

struct AJADevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "AJA";
    std::mutex mutex;

#ifdef HAS_AJA
    CNTV2Card device;
#endif
};

AJADevice::AJADevice() : impl_(std::make_unique<Impl>()) {}
AJADevice::~AJADevice() { close(); }

bool AJADevice::open(const DeviceConfig& config) {
#ifdef HAS_AJA
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening AJA device index=" +
                         std::to_string(config.deviceIndex));
    if (!CNTV2DeviceScanner::GetDeviceAtIndex(
            static_cast<ULWord>(config.deviceIndex), impl_->device)) {
        SDKLogger::error(TAG, "AJA device not found at index " +
                              std::to_string(config.deviceIndex));
        return false;
    }
    impl_->name = config.name.empty()
                  ? impl_->device.GetDisplayName()
                  : config.name;
    impl_->isOpen = true;
    SDKLogger::info(TAG, "Device opened: " + impl_->name);
    return true;
#else
    SDKLogger::warn(TAG, "AJA SDK not available — device will not open");
    return false;
#endif
}

void AJADevice::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing AJA device: " + impl_->name);
#ifdef HAS_AJA
    impl_->device.Close();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Device closed");
}

bool AJADevice::isOpen() const { return impl_->isOpen; }
std::string AJADevice::deviceName() const { return impl_->name; }
DeviceType AJADevice::deviceType() const { return DeviceType::BIDIRECTIONAL; }

std::vector<VideoMode> AJADevice::supportedModes() const {
    return {
        {1920, 1080, 25.0,  PixelFormat::UYVY, false},
        {1920, 1080, 29.97, PixelFormat::UYVY, false},
        {1920, 1080, 50.0,  PixelFormat::UYVY, false},
        {1920, 1080, 59.94, PixelFormat::UYVY, false},
        {3840, 2160, 25.0,  PixelFormat::V210, false},
        {3840, 2160, 29.97, PixelFormat::V210, false},
        {3840, 2160, 50.0,  PixelFormat::V210, false},
        {3840, 2160, 59.94, PixelFormat::V210, false},
    };
}

bool AJADevice::startCapture(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startCapture() — use AJAInput for capture");
    return false;
}
bool AJADevice::stopCapture() { return false; }
VideoFrame AJADevice::captureFrame() { return VideoFrame{}; }

bool AJADevice::startPlayout(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startPlayout() — use AJAOutput for playout");
    return false;
}
bool AJADevice::stopPlayout() { return false; }
bool AJADevice::sendFrame(const VideoFrame& /*frame*/) { return false; }

void AJADevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode AJADevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> AJADevice::enumerateDevices() {
#ifdef HAS_AJA
    std::vector<DeviceConfig> devices;
    CNTV2DeviceScanner scanner;
    const NTV2DeviceInfoList& infoList = scanner.GetDeviceInfoList();
    int idx = 0;
    for (const auto& info : infoList) {
        DeviceConfig cfg;
        cfg.deviceIndex = idx++;
        cfg.name        = info.deviceIdentifier;
        devices.push_back(cfg);
    }
    SDKLogger::info(TAG, "enumerateDevices() found " +
                         std::to_string(devices.size()) + " AJA device(s)");
    return devices;
#endif
    SDKLogger::debug(TAG, "enumerateDevices() — no AJA SDK, returning empty list");
    return {};
}

