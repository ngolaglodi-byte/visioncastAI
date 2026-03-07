# SDK Integration Guide

Internal documentation for the VisionCast-AI SDK abstraction layer integrations.

## Overview

The SDK layer provides a unified C++ API for professional broadcast hardware and network video devices. Each device is wrapped behind the `IVideoDevice` interface and, where appropriate, the role-specific `VideoInputInterface` / `VideoOutputInterface`.

## Architecture

```
                    ┌──────────────────────┐
                    │      IVideoDevice     │  (full bidirectional interface)
                    └──────────┬───────────┘
                               │
             ┌─────────────────┼─────────────────┐
             │                 │                  │
  ┌──────────▼──────┐  ┌──────▼──────┐  ┌───────▼──────┐
  │ VideoInputIface │  │ DeckLink    │  │ VideoOutput  │
  │ (capture only)  │  │ Device      │  │ Iface        │
  └─────┬───────────┘  │ (bidir.)   │  │ (playout)    │
        │               └─────────────┘  └──────┬───────┘
   ┌────┴────┐                              ┌───┴────┐
   │DeckLink │                              │DeckLink│
   │ Input   │                              │ Output │
   └─────────┘                              └────────┘
   │NDIInput │                              │NDIOutput│
   └─────────┘                              └─────────┘
```

## DeckLink Integration

### SDK Requirement

Place the Blackmagic DeckLink SDK headers in `sdk/decklink/include/`. CMake will auto-detect and set `HAS_DECKLINK=1`.

### Features

| Feature | DeckLinkInput | DeckLinkOutput |
|---|---|---|
| SDI connector | ✓ | ✓ |
| HDMI connector | ✓ | ✓ |
| 1080p (25/29.97/50/59.94) | ✓ | ✓ |
| 4K (25/29.97/50/59.94) | ✓ | ✓ |
| Low-latency mode | ✓ | ✓ |
| Timecode (SMPTE RP188) | ✓ (read) | ✓ (embed) |
| Genlock reference | — | ✓ |
| Signal detection | ✓ | — |
| Async frame callback | ✓ | — |

### Error Handling

DeckLink operations throw `DeckLinkError` (with HRESULT code) or `DeviceNotFoundError` when a device cannot be found. Signal loss raises `SignalLostError`. All exceptions inherit from `SDKError`.

### Example

```cpp
#include <visioncast_sdk/decklink_input.h>
#include <visioncast_sdk/sdk_error.h>
#include <visioncast_sdk/sdk_logger.h>

SDKLogger::setLevel(LogLevel::DEBUG);

DeckLinkInput input;
input.setConnector(DeckLinkConnector::SDI);
input.setLowLatency(true);

DeviceConfig cfg;
cfg.deviceIndex = 0;

try {
    if (!input.open(cfg))
        throw DeckLinkError("Failed to open device");

    VideoMode mode{1920, 1080, 25.0, PixelFormat::UYVY, false};
    input.startCapture(mode);

    // Push-mode capture
    input.setFrameCallback([](const VideoFrame& frame) {
        // Process frame...
        // Read timecode: input.lastTimecode()
    });

    // ... later
    input.stopCapture();
    input.close();
} catch (const SDKError& e) {
    SDKLogger::error("App", e.what());
}
```

## NDI Integration

### SDK Requirement

Place the NDI SDK headers in `sdk/ndi/include/`. CMake will auto-detect and set `HAS_NDI=1`.

### Features

| Feature | NDIInput | NDIOutput |
|---|---|---|
| Full NDI (uncompressed) | ✓ | ✓ |
| NDI HX (compressed) | ✓ | ✓ |
| 1080p (25/29.97/50/59.94) | ✓ | ✓ |
| 4K (25/29.97/50/59.94) | ✓ | ✓ |
| Network discovery | ✓ | — |
| Source name | — | ✓ |
| Timecode | ✓ (read) | ✓ (embed) |
| Signal detection | ✓ | — |
| Async frame callback | ✓ | — |

### Bandwidth Modes

```cpp
NDIInput input;
input.setBandwidth(NDIBandwidth::HX);   // Low-bandwidth compressed
input.setBandwidth(NDIBandwidth::FULL);  // Full-quality uncompressed
```

### Error Handling

NDI operations throw `NDIError` when the runtime is unavailable or connection fails. Network discovery failures are logged via `SDKLogger` at WARN level.

### Example

```cpp
#include <visioncast_sdk/ndi_input.h>
#include <visioncast_sdk/ndi_output.h>
#include <visioncast_sdk/sdk_error.h>
#include <visioncast_sdk/sdk_logger.h>

// Discover NDI sources on the network
auto sources = NDIInput::discoverSources();

NDIInput input;
input.setBandwidth(NDIBandwidth::FULL);

DeviceConfig cfg;
cfg.name = "Camera 1 (NDI)";

try {
    input.open(cfg);
    VideoMode mode{1920, 1080, 29.97, PixelFormat::UYVY, false};
    input.startCapture(mode);

    VideoFrame frame = input.captureFrame();
    std::string tc = input.lastTimecode(); // SMPTE timecode

    input.stopCapture();
    input.close();
} catch (const NDIError& e) {
    SDKLogger::error("App", e.what());
}

// NDI Output
NDIOutput output;
output.setSourceName("VisionCast PGM");
output.setBandwidth(NDIBandwidth::FULL);

DeviceConfig outCfg;
output.open(outCfg);
output.startPlayout({1920, 1080, 25.0, PixelFormat::UYVY, false});
output.setOutputTimecode("01:00:00:00");
output.sendFrame(frame);
output.stopPlayout();
output.close();
```

## Unified Interfaces

### VideoInputInterface

All input devices (DeckLinkInput, NDIInput) implement `VideoInputInterface`, which provides:

- `openInput()` / `closeInput()` / `isInputOpen()` — lifecycle
- `startCapture()` / `stopCapture()` / `captureFrame()` — capture
- `setFrameCallback()` — async push-mode capture
- `hasSignal()` / `detectedMode()` — signal detection
- `lastTimecode()` — SMPTE timecode from last captured frame

### VideoOutputInterface

All output devices (DeckLinkOutput, NDIOutput) implement `VideoOutputInterface`, which provides:

- `openOutput()` / `closeOutput()` / `isOutputOpen()` — lifecycle
- `startPlayout()` / `stopPlayout()` / `sendFrame()` — playout
- `isOutputActive()` — status check
- `setOutputTimecode()` — embed SMPTE timecode in output

### Polymorphic Usage

```cpp
void processInput(VideoInputInterface& input) {
    DeviceConfig cfg;
    input.openInput(cfg);
    input.startCapture({1920, 1080, 25.0, PixelFormat::UYVY, false});
    VideoFrame frame = input.captureFrame();
    std::string tc = input.lastTimecode();
    input.stopCapture();
    input.closeInput();
}

// Works with any input device:
DeckLinkInput decklink;
NDIInput ndi;
processInput(decklink);
processInput(ndi);
```

## Logging

All SDK components use `SDKLogger` for consistent, levelled logging:

```
[INFO ] [DeckLinkInput] Opened device: DeckLink Mini Recorder 4K
[WARN ] [NDIOutput] NDI SDK not available — device will not open
[ERROR] [DeckLinkOutput] startPlayout() called on closed device
```

Configure via:
```cpp
SDKLogger::setLevel(LogLevel::DEBUG);       // Minimum level
SDKLogger::setSink(myCustomLogFunction);    // Custom output
```
