/// @file decklink_input.cpp
/// @brief Blackmagic DeckLink capture-only implementation.
///
/// When HAS_DECKLINK is defined the DeckLink SDK is used for real hardware access.
/// Otherwise a safe stub is compiled so the library still links.

#include "visioncast_sdk/decklink_input.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"
#include "decklink_helpers.h"

#include <cstring>
#include <mutex>
#include <vector>

#ifdef HAS_DECKLINK
#include <objbase.h>
#endif

static const char* TAG = "DeckLinkInput";

// ---------------------------------------------------------------------------
// COM input-callback: stores the most-recently arrived video frame.
// ---------------------------------------------------------------------------
#ifdef HAS_DECKLINK

class DeckLinkInputCB final : public IDeckLinkInputCallback {
public:
    explicit DeckLinkInputCB() = default;

    // IDeckLinkInputCallback ------------------------------------------------
    HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(
        BMDVideoInputFormatChangedEvents /*events*/,
        IDeckLinkDisplayMode*           /*newMode*/,
        BMDDetectedVideoInputFormatFlags /*flags*/) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(
        IDeckLinkVideoInputFrame*  videoFrame,
        IDeckLinkAudioInputPacket* /*audioPacket*/) override
    {
        if (!videoFrame) return S_OK;

        std::lock_guard<std::mutex> lk(frameMutex_);
        width_  = videoFrame->GetWidth();
        height_ = videoFrame->GetHeight();
        stride_ = videoFrame->GetRowBytes();

        buf_.resize(static_cast<size_t>(stride_) * height_);
        void* src = nullptr;
        if (videoFrame->GetBytes(&src) == S_OK && src)
            std::memcpy(buf_.data(), src, buf_.size());

        // Timecode
        IDeckLinkTimecode* tc = nullptr;
        if (videoFrame->GetTimecode(bmdTimecodeRP188Any, &tc) == S_OK && tc) {
            BSTR tcStr = nullptr;
            if (tc->GetString(&tcStr) == S_OK && tcStr) {
                lastTimecode_ = bstrToString(tcStr);
                SysFreeString(tcStr);
            }
            tc->Release();
        }
        return S_OK;
    }

    // IUnknown --------------------------------------------------------------
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv) override {
        if (!ppv) return E_POINTER;
        if (iid == IID_IUnknown ||
            iid == IID_IDeckLinkInputCallback) {
            *ppv = this; AddRef(); return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef()  override { return InterlockedIncrement(&refCount_); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG rc = InterlockedDecrement(&refCount_);
        if (!rc) delete this;
        return rc;
    }

    // Accessors -------------------------------------------------------------
    VideoFrame getLastFrame(PixelFormat fmt) const {
        std::lock_guard<std::mutex> lk(frameMutex_);
        VideoFrame f;
        f.width  = width_;
        f.height = height_;
        f.stride = stride_;
        f.format = fmt;
        f.data   = const_cast<uint8_t*>(buf_.data());
        return f;
    }

    std::string lastTimecode() const {
        std::lock_guard<std::mutex> lk(frameMutex_);
        return lastTimecode_;
    }

private:
    mutable std::mutex   frameMutex_;
    std::vector<uint8_t> buf_;
    int                  width_  = 0;
    int                  height_ = 0;
    int                  stride_ = 0;
    std::string          lastTimecode_;
    volatile LONG        refCount_ = 1;
};

#endif // HAS_DECKLINK

// ---------------------------------------------------------------------------

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
    IDeckLink*              deckLink      = nullptr;
    IDeckLinkInput*         deckLinkInput = nullptr;
    IDeckLinkConfiguration* config        = nullptr;
    DeckLinkInputCB*        callback      = nullptr;
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

        if (deckLink->QueryInterface(IID_IDeckLinkInput,
                reinterpret_cast<void**>(&impl_->deckLinkInput)) != S_OK)
            throw DeckLinkError("Device does not support input");

        impl_->deckLink = deckLink;

        // Create and register the input callback
        impl_->callback = new DeckLinkInputCB();
        impl_->deckLinkInput->SetCallback(impl_->callback);

        BSTR nameStr = nullptr;
        if (deckLink->GetDisplayName(&nameStr) == S_OK && nameStr) {
            impl_->name = bstrToString(nameStr);
            SysFreeString(nameStr);
        } else {
            impl_->name = config.name.empty() ? "DeckLink Input" : config.name;
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

void DeckLinkInput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing device: " + impl_->name);
    if (impl_->capturing) {
        impl_->capturing = false;
#ifdef HAS_DECKLINK
        impl_->deckLinkInput->StopStreams();
        impl_->deckLinkInput->DisableVideoInput();
#endif
    }
#ifdef HAS_DECKLINK
    if (impl_->deckLinkInput) {
        impl_->deckLinkInput->SetCallback(nullptr);
        impl_->deckLinkInput->Release();
        impl_->deckLinkInput = nullptr;
    }
    if (impl_->callback)  { impl_->callback->Release();  impl_->callback  = nullptr; }
    if (impl_->deckLink)  { impl_->deckLink->Release();  impl_->deckLink  = nullptr; }
    CoUninitialize();
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
        BMDDisplayMode displayMode = resolveBMDMode(mode);
        BMDPixelFormat pixFmt = resolveBMDPixelFormat(mode.format);
        HRESULT hr = impl_->deckLinkInput->EnableVideoInput(displayMode, pixFmt,
            impl_->lowLatency ? bmdVideoInputEnableLowLatencyOutput
                              : bmdVideoInputFlagDefault);
        if (hr != S_OK) throw DeckLinkError("EnableVideoInput failed", hr);

        hr = impl_->deckLinkInput->StartStreams();
        if (hr != S_OK) throw DeckLinkError("StartStreams failed", hr);
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
    impl_->deckLinkInput->StopStreams();
    impl_->deckLinkInput->DisableVideoInput();
#endif
    impl_->capturing = false;
    return true;
}

VideoFrame DeckLinkInput::captureFrame() {
#ifdef HAS_DECKLINK
    if (!impl_->callback) {
        SDKLogger::debug(TAG, "captureFrame() — callback not registered");
        return VideoFrame{};
    }
    // The callback stores a copy of the last frame delivered by the SDK.
    VideoFrame frame = impl_->callback->getLastFrame(impl_->currentMode.format);
    // Update cached timecode from the callback
    impl_->lastTimecode = impl_->callback->lastTimecode();
    return frame;
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
    if (!impl_->deckLinkInput) return false;
    BMDDetectedVideoInputFormatFlags flags;
    BOOL interlaced = FALSE;
    BMDDisplayMode detectedMode = bmdModeUnknown;
    return impl_->deckLinkInput->GetCurrentDetectedVideoInputMode(
               &detectedMode, &flags, &interlaced) == S_OK;
#endif
    return false;
}

VideoMode DeckLinkInput::detectedMode() const {
#ifdef HAS_DECKLINK
    if (!impl_->deckLinkInput) return VideoMode{};
    BMDDetectedVideoInputFormatFlags flags;
    BOOL interlaced = FALSE;
    BMDDisplayMode bmdMode = bmdModeUnknown;
    if (impl_->deckLinkInput->GetCurrentDetectedVideoInputMode(
            &bmdMode, &flags, &interlaced) == S_OK) {
        return bmdModeToVideoMode(bmdMode, interlaced == TRUE);
    }
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
        // Only include devices that support input
        IDeckLinkInput* inp = nullptr;
        if (dl->QueryInterface(IID_IDeckLinkInput,
                reinterpret_cast<void**>(&inp)) == S_OK) {
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
            inp->Release();
        }
        dl->Release();
        ++idx;
    }
    it->Release();
    CoUninitialize();
    SDKLogger::info(TAG, "enumerateDevices() found " +
                         std::to_string(devices.size()) + " capture device(s)");
    return devices;
#endif
    SDKLogger::debug(TAG, "enumerateDevices() — no SDK, returning empty list");
    return {};
}
