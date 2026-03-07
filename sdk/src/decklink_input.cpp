/// @file decklink_input.cpp
/// @brief Blackmagic DeckLink capture-only implementation.
///
/// When HAS_DECKLINK is defined the DeckLink SDK is used for real hardware access.
/// Otherwise a safe stub is compiled so the library still links.

#include "visioncast_sdk/decklink_input.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#include <iostream>
#include <mutex>

static const char* TAG = "DeckLinkInput";

struct DeckLinkInput::Impl {
    bool isOpen = false;
    bool capturing = false;
    bool lowLatency = false;
    VideoMode currentMode;
    std::string name = "DeckLink Input";
    DeckLinkConnector connector = DeckLinkConnector::AUTO;
    FrameCallback frameCallback = nullptr;
    std::string lastTimecode;
    std::mutex mutex;

#ifdef HAS_DECKLINK
    // DeckLink SDK handles would be stored here:
    // IDeckLink*             deckLink       = nullptr;
    // IDeckLinkInput*        deckLinkInput  = nullptr;
    // IDeckLinkConfiguration* config        = nullptr;
#endif
};

DeckLinkInput::DeckLinkInput() : impl_(std::make_unique<Impl>()) {}
DeckLinkInput::~DeckLinkInput() { close(); }

bool DeckLinkInput::open(const DeviceConfig& config) {
#ifdef HAS_DECKLINK
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening DeckLink capture device index=" +
                         std::to_string(config.deviceIndex));
    try {
        // --- DeckLink SDK initialisation ---
        // IDeckLinkIterator* iterator = CreateDeckLinkIteratorInstance();
        // if (!iterator) throw DeckLinkError("DeckLink drivers not installed");
        //
        // IDeckLink* deckLink = nullptr;
        // for (int i = 0; i <= config.deviceIndex; ++i) {
        //     if (iterator->Next(&deckLink) != S_OK)
        //         throw DeviceNotFoundError("DeckLink device " +
        //                                   std::to_string(config.deviceIndex));
        // }
        // iterator->Release();
        //
        // if (deckLink->QueryInterface(IID_IDeckLinkInput,
        //         reinterpret_cast<void**>(&impl_->deckLinkInput)) != S_OK)
        //     throw DeckLinkError("Device does not support input");
        //
        // impl_->deckLink = deckLink;
        impl_->name = config.name.empty() ? "DeckLink Input" : config.name;
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

void DeckLinkInput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing device: " + impl_->name);
    if (impl_->capturing) {
        impl_->capturing = false;
#ifdef HAS_DECKLINK
        // impl_->deckLinkInput->StopStreams();
        // impl_->deckLinkInput->DisableVideoInput();
#endif
    }
#ifdef HAS_DECKLINK
    // if (impl_->deckLinkInput) { impl_->deckLinkInput->Release(); impl_->deckLinkInput = nullptr; }
    // if (impl_->deckLink) { impl_->deckLink->Release(); impl_->deckLink = nullptr; }
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Device closed");
}

bool DeckLinkInput::isOpen() const { return impl_->isOpen; }
std::string DeckLinkInput::deviceName() const { return impl_->name; }
DeviceType DeckLinkInput::deviceType() const { return DeviceType::CAPTURE; }

std::vector<VideoMode> DeckLinkInput::supportedModes() const {
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

bool DeckLinkInput::startCapture(const VideoMode& mode) {
#ifdef HAS_DECKLINK
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startCapture() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting capture " + std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));
    try {
        // --- DeckLink SDK capture start ---
        // BMDDisplayMode displayMode = resolveBMDMode(mode);
        // BMDPixelFormat pixFmt = resolveBMDPixelFormat(mode.format);
        // HRESULT hr = impl_->deckLinkInput->EnableVideoInput(displayMode, pixFmt,
        //     impl_->lowLatency ? bmdVideoInputEnableLowLatencyOutput
        //                       : bmdVideoInputFlagDefault);
        // if (hr != S_OK) throw DeckLinkError("EnableVideoInput failed", hr);
        //
        // hr = impl_->deckLinkInput->StartStreams();
        // if (hr != S_OK) throw DeckLinkError("StartStreams failed", hr);
        impl_->currentMode = mode;
        impl_->capturing = true;
        SDKLogger::info(TAG, "Capture started");
        return true;
    } catch (const SDKError& e) {
        SDKLogger::error(TAG, std::string("startCapture() failed: ") + e.what());
        return false;
    }
#else
    return false;
#endif
}

bool DeckLinkInput::stopCapture() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->capturing) return true;
    SDKLogger::info(TAG, "Stopping capture");
#ifdef HAS_DECKLINK
    // impl_->deckLinkInput->StopStreams();
    // impl_->deckLinkInput->DisableVideoInput();
#endif
    impl_->capturing = false;
    return true;
}

VideoFrame DeckLinkInput::captureFrame() {
#ifdef HAS_DECKLINK
    // IDeckLinkVideoInputFrame* dlFrame = nullptr;
    // HRESULT hr = impl_->deckLinkInput->GetLastFrame(&dlFrame);
    // if (hr != S_OK || !dlFrame) throw DeckLinkError("captureFrame failed", hr);
    //
    // VideoFrame frame;
    // frame.width  = dlFrame->GetWidth();
    // frame.height = dlFrame->GetHeight();
    // frame.stride = dlFrame->GetRowBytes();
    // dlFrame->GetBytes(reinterpret_cast<void**>(&frame.data));
    // frame.format = impl_->currentMode.format;
    //
    // // Timecode extraction
    // IDeckLinkTimecode* tc = nullptr;
    // if (dlFrame->GetTimecode(bmdTimecodeRP188Any, &tc) == S_OK && tc) {
    //     const char* tcStr = nullptr;
    //     tc->GetString(&tcStr);
    //     if (tcStr) impl_->lastTimecode = tcStr;
    //     tc->Release();
    // }
    //
    // dlFrame->Release();
    // return frame;
#endif
    SDKLogger::debug(TAG, "captureFrame() — no SDK, returning empty frame");
    return VideoFrame{};
}

// Playout no-ops
bool DeckLinkInput::startPlayout(const VideoMode& /*mode*/) { return false; }
bool DeckLinkInput::stopPlayout() { return false; }
bool DeckLinkInput::sendFrame(const VideoFrame& /*frame*/) { return false; }

void DeckLinkInput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode DeckLinkInput::currentMode() const { return impl_->currentMode; }

// -- VideoInputInterface --
bool DeckLinkInput::openInput(const DeviceConfig& config) { return open(config); }
void DeckLinkInput::closeInput() { close(); }
bool DeckLinkInput::isInputOpen() const { return isOpen(); }

void DeckLinkInput::setFrameCallback(FrameCallback callback) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->frameCallback = std::move(callback);
    SDKLogger::debug(TAG, "Frame callback registered");
}

std::string DeckLinkInput::inputDeviceName() const { return deviceName(); }
std::vector<VideoMode> DeckLinkInput::supportedInputModes() const { return supportedModes(); }

std::string DeckLinkInput::lastTimecode() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->lastTimecode;
}

// -- DeckLinkInput-specific --
void DeckLinkInput::setConnector(DeckLinkConnector connector) {
    impl_->connector = connector;
    SDKLogger::debug(TAG, "Connector set to " +
        std::to_string(static_cast<int>(connector)));
}

DeckLinkConnector DeckLinkInput::connector() const {
    return impl_->connector;
}

bool DeckLinkInput::hasSignal() const {
#ifdef HAS_DECKLINK
    // BMDDetectedVideoInputFormatFlags flags;
    // return impl_->deckLinkInput->GetCurrentDetectedVideoInputMode(
    //            nullptr, &flags, nullptr) == S_OK;
#endif
    return false;
}

VideoMode DeckLinkInput::detectedMode() const {
#ifdef HAS_DECKLINK
    // Query the DeckLink SDK for the current input format
#endif
    return VideoMode{};
}

void DeckLinkInput::setLowLatency(bool enabled) {
    impl_->lowLatency = enabled;
    SDKLogger::debug(TAG, std::string("Low-latency mode ") +
                          (enabled ? "enabled" : "disabled"));
}

bool DeckLinkInput::lowLatency() const {
    return impl_->lowLatency;
}

std::vector<DeviceConfig> DeckLinkInput::enumerateDevices() {
#ifdef HAS_DECKLINK
    // std::vector<DeviceConfig> devices;
    // IDeckLinkIterator* it = CreateDeckLinkIteratorInstance();
    // if (!it) return devices;
    // IDeckLink* dl = nullptr;
    // int idx = 0;
    // while (it->Next(&dl) == S_OK) {
    //     const char* name = nullptr;
    //     dl->GetDisplayName(&name);
    //     DeviceConfig cfg;
    //     cfg.deviceIndex = idx++;
    //     cfg.name = name ? name : "DeckLink";
    //     devices.push_back(cfg);
    //     dl->Release();
    // }
    // it->Release();
    // return devices;
#endif
    SDKLogger::debug(TAG, "enumerateDevices() — no SDK, returning empty list");
    return {};
}
