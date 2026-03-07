/// @file aja_input.cpp
/// @brief AJA capture-only implementation.

#include "visioncast_sdk/aja_input.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_AJA
#include "ntv2device.h"
#include "ntv2devicescanner.h"
#include "ntv2autocirculate.h"
#include "ntv2utils.h"
#include "ntv2signalrouter.h"
#endif

#include <mutex>

static const char* TAG = "AJAInput";

struct AJAInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    VideoMode currentMode;
    std::string name = "AJA Input";
    int channel = 0;
    std::mutex mutex;

#ifdef HAS_AJA
    CNTV2Card device;
#endif
};

AJAInput::AJAInput() : impl_(std::make_unique<Impl>()) {}
AJAInput::~AJAInput() { close(); }

bool AJAInput::open(const DeviceConfig& config) {
#ifdef HAS_AJA
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening AJA capture device index=" +
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
    SDKLogger::info(TAG, "Capture device opened: " + impl_->name);
    return true;
#else
    SDKLogger::warn(TAG, "AJA SDK not available — device will not open");
    return false;
#endif
}

void AJAInput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing AJA capture device: " + impl_->name);
    if (impl_->capturing) {
#ifdef HAS_AJA
        NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
        impl_->device.AutoCirculateStop(ch);
#endif
        impl_->capturing = false;
    }
#ifdef HAS_AJA
    impl_->device.Close();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Capture device closed");
}

bool AJAInput::isOpen() const { return impl_->isOpen; }
std::string AJAInput::deviceName() const { return impl_->name; }
DeviceType AJAInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> AJAInput::supportedModes() const {
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

bool AJAInput::startCapture(const VideoMode& mode) {
#ifdef HAS_AJA
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startCapture() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting AJA capture " +
                         std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);

    // Initialise AutoCirculate for input with RP-188 timecode support.
    if (!impl_->device.AutoCirculateInitForInput(
            ch,
            7 /*numFrames*/,
            NTV2_AUDIO_SYSTEM_1,
            AUTOCIRCULATE_WITH_RP188)) {
        SDKLogger::error(TAG, "AutoCirculateInitForInput failed");
        return false;
    }
    if (!impl_->device.AutoCirculateStart(ch)) {
        SDKLogger::error(TAG, "AutoCirculateStart failed");
        return false;
    }
    impl_->currentMode = mode;
    impl_->capturing = true;
    SDKLogger::info(TAG, "AJA capture started");
    return true;
#else
    return false;
#endif
}

bool AJAInput::stopCapture() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->capturing) return true;
    SDKLogger::info(TAG, "Stopping AJA capture");
#ifdef HAS_AJA
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
    impl_->device.AutoCirculateStop(ch);
#endif
    impl_->capturing = false;
    return true;
}

VideoFrame AJAInput::captureFrame() {
#ifdef HAS_AJA
    if (!impl_->capturing) return VideoFrame{};
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
    AUTOCIRCULATE_TRANSFER transfer;
    if (!impl_->device.AutoCirculateTransfer(ch, transfer)) {
        SDKLogger::debug(TAG, "captureFrame() — AutoCirculateTransfer failed");
        return VideoFrame{};
    }
    VideoFrame frame;
    frame.data   = reinterpret_cast<uint8_t*>(
                       transfer.acVideoBuffer.GetHostPointer());
    frame.width  = impl_->currentMode.width;
    frame.height = impl_->currentMode.height;
    // Derive stride from the actual buffer size for safety
    const ULWord byteCount = transfer.acVideoBuffer.GetByteCount();
    frame.stride = (impl_->currentMode.height > 0)
                   ? static_cast<int>(byteCount / impl_->currentMode.height)
                   : 0;
    frame.format = impl_->currentMode.format;
    return frame;
#endif
    SDKLogger::debug(TAG, "captureFrame() — no AJA SDK, returning empty frame");
    return VideoFrame{};
}

// Playout no-ops
bool AJAInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool AJAInput::stopPlayout() { return false; }
bool AJAInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void AJAInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode AJAInput::currentMode() const { return impl_->currentMode; }

// AJAInput-specific
void AJAInput::setChannel(int channel) {
    impl_->channel = channel;
}

int AJAInput::channel() const {
    return impl_->channel;
}

bool AJAInput::hasSignal() const {
#ifdef HAS_AJA
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
    NTV2VideoFormat fmt = NTV2_FORMAT_UNKNOWN;
    impl_->device.GetInputVideoFormat(fmt, ch);
    return fmt != NTV2_FORMAT_UNKNOWN;
#endif
    return false;
}

VideoMode AJAInput::detectedMode() const {
#ifdef HAS_AJA
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
    NTV2VideoFormat fmt = NTV2_FORMAT_UNKNOWN;
    if (!impl_->device.GetInputVideoFormat(fmt, ch) ||
        fmt == NTV2_FORMAT_UNKNOWN)
        return VideoMode{};
    // Map common NTV2 formats to VideoMode
    switch (fmt) {
        case NTV2_FORMAT_1080p_2500:   return {1920, 1080, 25.0,  PixelFormat::UYVY, false};
        case NTV2_FORMAT_1080p_2997:   return {1920, 1080, 29.97, PixelFormat::UYVY, false};
        case NTV2_FORMAT_1080p_5000_A: return {1920, 1080, 50.0,  PixelFormat::UYVY, false};
        case NTV2_FORMAT_1080p_5994_A: return {1920, 1080, 59.94, PixelFormat::UYVY, false};
        case NTV2_FORMAT_4x1920x1080p_2500: return {3840, 2160, 25.0,  PixelFormat::V210, false};
        case NTV2_FORMAT_4x1920x1080p_2997: return {3840, 2160, 29.97, PixelFormat::V210, false};
        case NTV2_FORMAT_4x1920x1080p_5000: return {3840, 2160, 50.0,  PixelFormat::V210, false};
        case NTV2_FORMAT_4x1920x1080p_5994: return {3840, 2160, 59.94, PixelFormat::V210, false};
        default: return VideoMode{};
    }
#endif
    return VideoMode{};
}

std::vector<DeviceConfig> AJAInput::enumerateDevices() {
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

