/// @file decklink_output.cpp
/// @brief Blackmagic DeckLink playout-only implementation.
///
/// When HAS_DECKLINK is defined the DeckLink SDK is used for real hardware access.
/// Otherwise a safe stub is compiled so the library still links.

#include "visioncast_sdk/decklink_output.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"
#include "decklink_helpers.h"

#include <cstdio>
#include <cstring>
#include <mutex>

#ifdef HAS_DECKLINK
#include <objbase.h>
#endif

static const char* TAG = "DeckLinkOutput";

struct DeckLinkOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    bool lowLatency = false;
    VideoMode currentMode;
    std::string name = "DeckLink Output";
    DeckLinkConnector connector = DeckLinkConnector::AUTO;
    DeckLinkReference reference = DeckLinkReference::FREE_RUN;
    std::string outputTimecode;
    std::mutex mutex;
    BMDTimeValue frameCounter = 0;  // running frame clock for scheduling

#ifdef HAS_DECKLINK
    IDeckLink*              deckLink       = nullptr;
    IDeckLinkOutput*        deckLinkOutput = nullptr;
    IDeckLinkConfiguration* config         = nullptr;
#endif
};

DeckLinkOutput::DeckLinkOutput() : impl_(std::make_unique<Impl>()) {}
DeckLinkOutput::~DeckLinkOutput() { close(); }

bool DeckLinkOutput::open(const DeviceConfig& config) {
#ifdef HAS_DECKLINK
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening DeckLink playout device index=" +
                         std::to_string(config.deviceIndex));
    try {
        CoInitialize(nullptr);

        // --- DeckLink SDK initialisation ---
        IDeckLinkIterator* iterator = CreateDeckLinkIteratorInstance();
        if (!iterator) throw DeckLinkError("DeckLink drivers not installed");

        IDeckLink* deckLink = nullptr;
        for (int i = 0; i <= config.deviceIndex; ++i) {
            if (impl_->deckLink) { impl_->deckLink->Release(); impl_->deckLink = nullptr; }
            if (iterator->Next(&deckLink) != S_OK)
                throw DeviceNotFoundError("DeckLink device " +
                                          std::to_string(config.deviceIndex));
        }
        iterator->Release();

        if (deckLink->QueryInterface(IID_IDeckLinkOutput,
                reinterpret_cast<void**>(&impl_->deckLinkOutput)) != S_OK)
            throw DeckLinkError("Device does not support output");

        impl_->deckLink = deckLink;

        BSTR nameStr = nullptr;
        if (deckLink->GetDisplayName(&nameStr) == S_OK && nameStr) {
            impl_->name = bstrToString(nameStr);
            SysFreeString(nameStr);
        } else {
            impl_->name = config.name.empty() ? "DeckLink Output" : config.name;
        }
        impl_->isOpen = true;
        SDKLogger::info(TAG, "Device opened: " + impl_->name);
        return true;
    } catch (const SDKError& e) {
        SDKLogger::error(TAG, std::string("open() failed: ") + e.what());
        return false;
    }
#else
    SDKLogger::warn(TAG, "DeckLink SDK not available — device will not open");
    return false;
#endif
}

void DeckLinkOutput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing device: " + impl_->name);
    if (impl_->playing) {
        impl_->playing = false;
#ifdef HAS_DECKLINK
        impl_->deckLinkOutput->StopScheduledPlayback(0, nullptr, 0);
        impl_->deckLinkOutput->DisableVideoOutput();
#endif
    }
#ifdef HAS_DECKLINK
    if (impl_->deckLinkOutput) { impl_->deckLinkOutput->Release(); impl_->deckLinkOutput = nullptr; }
    if (impl_->deckLink)       { impl_->deckLink->Release();       impl_->deckLink       = nullptr; }
    CoUninitialize();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Device closed");
}

bool DeckLinkOutput::isOpen() const { return impl_->isOpen; }
std::string DeckLinkOutput::deviceName() const { return impl_->name; }
DeviceType DeckLinkOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> DeckLinkOutput::supportedModes() const {
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
bool DeckLinkOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool DeckLinkOutput::stopCapture() { return false; }
VideoFrame DeckLinkOutput::captureFrame() { return VideoFrame{}; }

bool DeckLinkOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_DECKLINK
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startPlayout() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting playout " + std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));
    try {
        // --- DeckLink SDK playout start ---
        BMDDisplayMode displayMode = resolveBMDMode(mode);
        BMDPixelFormat pixFmt = resolveBMDPixelFormat(mode.format);
        HRESULT hr = impl_->deckLinkOutput->EnableVideoOutput(displayMode,
            bmdVideoOutputFlagDefault);
        if (hr != S_OK) throw DeckLinkError("EnableVideoOutput failed", hr);

        BMDTimeValue duration;
        BMDTimeScale  scale;
        getFrameTiming(mode.frameRate, duration, scale);
        hr = impl_->deckLinkOutput->StartScheduledPlayback(0, scale, 1.0);
        if (hr != S_OK) throw DeckLinkError("StartScheduledPlayback failed", hr);

        impl_->currentMode = mode;
        impl_->frameCounter = 0;
        impl_->playing = true;
        SDKLogger::info(TAG, "Playout started");
        return true;
    } catch (const SDKError& e) {
        SDKLogger::error(TAG, std::string("startPlayout() failed: ") + e.what());
        return false;
    }
#else
    return false;
#endif
}

bool DeckLinkOutput::stopPlayout() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->playing) return true;
    SDKLogger::info(TAG, "Stopping playout");
#ifdef HAS_DECKLINK
    impl_->deckLinkOutput->StopScheduledPlayback(0, nullptr, 0);
    impl_->deckLinkOutput->DisableVideoOutput();
#endif
    impl_->playing = false;
    return true;
}

bool DeckLinkOutput::sendFrame(const VideoFrame& frame) {
#ifdef HAS_DECKLINK
    if (!impl_->playing) return false;
    try {
        IDeckLinkMutableVideoFrame* dlFrame = nullptr;
        BMDPixelFormat pixFmt = resolveBMDPixelFormat(frame.format);
        HRESULT hr = impl_->deckLinkOutput->CreateVideoFrame(
            frame.width, frame.height, frame.stride,
            pixFmt, bmdFrameFlagDefault, &dlFrame);
        if (hr != S_OK || !dlFrame) throw DeckLinkError("CreateVideoFrame failed", hr);

        void* buf = nullptr;
        dlFrame->GetBytes(&buf);
        if (buf && frame.data)
            std::memcpy(buf, frame.data,
                        static_cast<size_t>(frame.stride) * frame.height);

        // Embed timecode if set
        if (!impl_->outputTimecode.empty()) {
            int hh = 0, mm = 0, ss = 0, ff = 0;
            std::sscanf(impl_->outputTimecode.c_str(),
                        "%d:%d:%d:%d", &hh, &mm, &ss, &ff);
            IDeckLinkTimecode* tc = nullptr;
            if (impl_->deckLinkOutput->CreateTimecode(
                    bmdTimecodeRP188LTC, &tc) == S_OK && tc) {
                tc->SetComponents(static_cast<uint8_t>(hh),
                                  static_cast<uint8_t>(mm),
                                  static_cast<uint8_t>(ss),
                                  static_cast<uint8_t>(ff));
                dlFrame->SetTimecode(bmdTimecodeRP188LTC, tc);
                tc->Release();
            }
        }

        BMDTimeValue duration;
        BMDTimeScale  scale;
        getFrameTiming(impl_->currentMode.frameRate, duration, scale);
        BMDTimeValue displayTime = impl_->frameCounter * duration;
        hr = impl_->deckLinkOutput->ScheduleVideoFrame(
                 dlFrame, displayTime, duration, scale);
        dlFrame->Release();
        if (hr != S_OK) throw DeckLinkError("ScheduleVideoFrame failed", hr);
        ++impl_->frameCounter;
        return true;
    } catch (const SDKError& e) {
        SDKLogger::error(TAG, std::string("sendFrame() failed: ") + e.what());
        return false;
    }
#endif
    (void)frame;
    return impl_->playing;
}

void DeckLinkOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode DeckLinkOutput::currentMode() const { return impl_->currentMode; }

// -- VideoOutputInterface --
bool DeckLinkOutput::openOutput(const DeviceConfig& config) { return open(config); }
void DeckLinkOutput::closeOutput() { close(); }
bool DeckLinkOutput::isOutputOpen() const { return isOpen(); }

bool DeckLinkOutput::isOutputActive() const {
    return impl_->playing;
}

std::string DeckLinkOutput::outputDeviceName() const { return deviceName(); }
std::vector<VideoMode> DeckLinkOutput::supportedOutputModes() const { return supportedModes(); }

void DeckLinkOutput::setOutputTimecode(const std::string& timecode) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->outputTimecode = timecode;
    SDKLogger::debug(TAG, "Output timecode set: " + timecode);
}

// -- DeckLinkOutput-specific --
void DeckLinkOutput::setConnector(DeckLinkConnector connector) {
    impl_->connector = connector;
    SDKLogger::debug(TAG, "Connector set to " +
        std::to_string(static_cast<int>(connector)));
}

DeckLinkConnector DeckLinkOutput::connector() const {
    return impl_->connector;
}

void DeckLinkOutput::setReferenceSource(DeckLinkReference ref) {
    impl_->reference = ref;
    SDKLogger::debug(TAG, "Reference source set to " +
        std::to_string(static_cast<int>(ref)));
}

DeckLinkReference DeckLinkOutput::referenceSource() const {
    return impl_->reference;
}

void DeckLinkOutput::setLowLatency(bool enabled) {
    impl_->lowLatency = enabled;
    SDKLogger::debug(TAG, std::string("Low-latency mode ") +
                          (enabled ? "enabled" : "disabled"));
}

bool DeckLinkOutput::lowLatency() const {
    return impl_->lowLatency;
}

std::vector<DeviceConfig> DeckLinkOutput::enumerateDevices() {
#ifdef HAS_DECKLINK
    std::vector<DeviceConfig> devices;
    CoInitialize(nullptr);
    IDeckLinkIterator* it = CreateDeckLinkIteratorInstance();
    if (!it) {
        SDKLogger::warn(TAG, "enumerateDevices() — DeckLink drivers not installed");
        CoUninitialize();
        return devices;
    }
    IDeckLink* dl = nullptr;
    int idx = 0;
    while (it->Next(&dl) == S_OK) {
        // Only include devices that support output
        IDeckLinkOutput* out = nullptr;
        if (dl->QueryInterface(IID_IDeckLinkOutput,
                reinterpret_cast<void**>(&out)) == S_OK) {
            BSTR nameStr = nullptr;
            DeviceConfig cfg;
            cfg.deviceIndex = idx;
            if (dl->GetDisplayName(&nameStr) == S_OK && nameStr) {
                cfg.name = bstrToString(nameStr);
                SysFreeString(nameStr);
            } else {
                cfg.name = "DeckLink";
            }
            devices.push_back(cfg);
            out->Release();
        }
        dl->Release();
        ++idx;
    }
    it->Release();
    CoUninitialize();
    SDKLogger::info(TAG, "enumerateDevices() found " +
                         std::to_string(devices.size()) + " playout device(s)");
    return devices;
#endif
    SDKLogger::debug(TAG, "enumerateDevices() — no SDK, returning empty list");
    return {};
}
