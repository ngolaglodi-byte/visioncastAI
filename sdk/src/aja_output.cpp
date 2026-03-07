/// @file aja_output.cpp
/// @brief AJA playout-only implementation.

#include "visioncast_sdk/aja_output.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_AJA
#include "ntv2device.h"
#include "ntv2devicescanner.h"
#include "ntv2autocirculate.h"
#include "ntv2utils.h"
#include "ntv2signalrouter.h"
#endif

#include <cstring>
#include <mutex>

static const char* TAG = "AJAOutput";

struct AJAOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "AJA Output";
    int channel = 0;
    AJAReference reference = AJAReference::FREE_RUN;
    std::mutex mutex;

#ifdef HAS_AJA
    CNTV2Card device;
#endif
};

AJAOutput::AJAOutput() : impl_(std::make_unique<Impl>()) {}
AJAOutput::~AJAOutput() { close(); }

bool AJAOutput::open(const DeviceConfig& config) {
#ifdef HAS_AJA
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening AJA playout device index=" +
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
    SDKLogger::info(TAG, "Playout device opened: " + impl_->name);
    return true;
#else
    SDKLogger::warn(TAG, "AJA SDK not available — device will not open");
    return false;
#endif
}

void AJAOutput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing AJA playout device: " + impl_->name);
    if (impl_->playing) {
#ifdef HAS_AJA
        NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
        impl_->device.AutoCirculateStop(ch);
#endif
        impl_->playing = false;
    }
#ifdef HAS_AJA
    impl_->device.Close();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Playout device closed");
}

bool AJAOutput::isOpen() const { return impl_->isOpen; }
std::string AJAOutput::deviceName() const { return impl_->name; }
DeviceType AJAOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> AJAOutput::supportedModes() const {
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

// Capture no-ops
bool AJAOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool AJAOutput::stopCapture() { return false; }
VideoFrame AJAOutput::captureFrame() { return VideoFrame{}; }

bool AJAOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_AJA
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startPlayout() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting AJA playout " +
                         std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);

    // Initialise AutoCirculate for output with RP-188 timecode support.
    if (!impl_->device.AutoCirculateInitForOutput(
            ch,
            7 /*numFrames*/,
            NTV2_AUDIO_SYSTEM_1,
            AUTOCIRCULATE_WITH_RP188)) {
        SDKLogger::error(TAG, "AutoCirculateInitForOutput failed");
        return false;
    }
    if (!impl_->device.AutoCirculateStart(ch)) {
        SDKLogger::error(TAG, "AutoCirculateStart failed");
        return false;
    }
    impl_->currentMode = mode;
    impl_->playing = true;
    SDKLogger::info(TAG, "AJA playout started");
    return true;
#else
    return false;
#endif
}

bool AJAOutput::stopPlayout() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->playing) return true;
    SDKLogger::info(TAG, "Stopping AJA playout");
#ifdef HAS_AJA
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
    impl_->device.AutoCirculateStop(ch);
#endif
    impl_->playing = false;
    return true;
}

bool AJAOutput::sendFrame(const VideoFrame& frame) {
#ifdef HAS_AJA
    if (!impl_->playing) return false;
    NTV2Channel ch = static_cast<NTV2Channel>(impl_->channel);
    AUTOCIRCULATE_TRANSFER transfer;
    const size_t bufSize =
        static_cast<size_t>(frame.stride) * frame.height;
    transfer.acVideoBuffer.Set(frame.data, static_cast<ULWord>(bufSize));
    if (!impl_->device.AutoCirculateTransfer(ch, transfer)) {
        SDKLogger::debug(TAG, "sendFrame() — AutoCirculateTransfer failed");
        return false;
    }
    return true;
#endif
    (void)frame;
    return impl_->playing;
}

void AJAOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode AJAOutput::currentMode() const { return impl_->currentMode; }

// AJAOutput-specific
void AJAOutput::setChannel(int channel) {
    impl_->channel = channel;
}

int AJAOutput::channel() const {
    return impl_->channel;
}

void AJAOutput::setReferenceSource(AJAReference ref) {
    impl_->reference = ref;
}

AJAReference AJAOutput::referenceSource() const {
    return impl_->reference;
}

bool AJAOutput::isOutputActive() const {
    return impl_->playing;
}

std::vector<DeviceConfig> AJAOutput::enumerateDevices() {
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

