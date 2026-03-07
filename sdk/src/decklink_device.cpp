/// @file decklink_device.cpp
/// @brief Blackmagic DeckLink device implementation.
///
/// Generic bidirectional DeckLink wrapper (used when a device serves both
/// capture and playout roles).  For role-specific use, prefer DeckLinkInput
/// or DeckLinkOutput which implement the unified VideoInputInterface /
/// VideoOutputInterface contracts.
///
/// When HAS_DECKLINK is defined the DeckLink SDK is used for real hardware
/// access.  Otherwise a safe stub is compiled so the library still links.

#include "visioncast_sdk/decklink_device.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"
#include "decklink_helpers.h"

#include <mutex>

#ifdef HAS_DECKLINK
#include <objbase.h>   // CoInitialize / CoUninitialize
#endif

static const char* TAG = "DeckLinkDevice";

struct DeckLinkDevice::Impl {
    bool isOpen = false;
    VideoMode currentMode;
    std::string name = "DeckLink";
    std::mutex mutex;

#ifdef HAS_DECKLINK
    IDeckLink*              deckLink = nullptr;
    IDeckLinkInput*         input    = nullptr;
    IDeckLinkOutput*        output   = nullptr;
    IDeckLinkConfiguration* config   = nullptr;
#endif
};

DeckLinkDevice::DeckLinkDevice() : impl_(std::make_unique<Impl>()) {}
DeckLinkDevice::~DeckLinkDevice() { close(); }

bool DeckLinkDevice::open(const DeviceConfig& config) {
#ifdef HAS_DECKLINK
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening DeckLink device index=" +
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
        impl_->deckLink = deckLink;

        // Query input capability (optional — device may be output-only)
        deckLink->QueryInterface(IID_IDeckLinkInput,
            reinterpret_cast<void**>(&impl_->input));
        // Query output capability (optional — device may be input-only)
        deckLink->QueryInterface(IID_IDeckLinkOutput,
            reinterpret_cast<void**>(&impl_->output));

        BSTR nameStr = nullptr;
        if (deckLink->GetDisplayName(&nameStr) == S_OK && nameStr) {
            impl_->name = bstrToString(nameStr);
            SysFreeString(nameStr);
        } else {
            impl_->name = config.name.empty() ? "DeckLink" : config.name;
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

void DeckLinkDevice::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing device: " + impl_->name);
#ifdef HAS_DECKLINK
    if (impl_->input)   { impl_->input->Release();   impl_->input   = nullptr; }
    if (impl_->output)  { impl_->output->Release();  impl_->output  = nullptr; }
    if (impl_->deckLink){ impl_->deckLink->Release(); impl_->deckLink= nullptr; }
    CoUninitialize();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "Device closed");
}

bool DeckLinkDevice::isOpen() const { return impl_->isOpen; }
std::string DeckLinkDevice::deviceName() const { return impl_->name; }
DeviceType DeckLinkDevice::deviceType() const { return DeviceType::BIDIRECTIONAL; }

std::vector<VideoMode> DeckLinkDevice::supportedModes() const {
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

bool DeckLinkDevice::startCapture(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startCapture() — use DeckLinkInput for capture");
    return false;
}
bool DeckLinkDevice::stopCapture() { return false; }
VideoFrame DeckLinkDevice::captureFrame() { return VideoFrame{}; }

bool DeckLinkDevice::startPlayout(const VideoMode& /*mode*/) {
    SDKLogger::debug(TAG, "startPlayout() — use DeckLinkOutput for playout");
    return false;
}
bool DeckLinkDevice::stopPlayout() { return false; }
bool DeckLinkDevice::sendFrame(const VideoFrame& /*frame*/) { return false; }

void DeckLinkDevice::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode DeckLinkDevice::currentMode() const { return impl_->currentMode; }

std::vector<DeviceConfig> DeckLinkDevice::enumerateDevices() {
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
        BSTR nameStr = nullptr;
        DeviceConfig cfg;
        cfg.deviceIndex = idx++;
        if (dl->GetDisplayName(&nameStr) == S_OK && nameStr) {
            cfg.name = bstrToString(nameStr);
            SysFreeString(nameStr);
        } else {
            cfg.name = "DeckLink";
        }
        devices.push_back(cfg);
        dl->Release();
    }
    it->Release();
    CoUninitialize();
    SDKLogger::info(TAG, "enumerateDevices() found " +
                         std::to_string(devices.size()) + " device(s)");
    return devices;
#endif
    SDKLogger::debug(TAG, "enumerateDevices() — no SDK, returning empty list");
    return {};
}
