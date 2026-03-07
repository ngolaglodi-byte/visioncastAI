# SDK — Hardware Abstraction Layer

Unified C++ interface for professional video capture and playout hardware.

## Abstract Interface

All devices implement `IVideoDevice` (defined in `include/visioncast_sdk/video_device.h`), providing a common API for capture, playout, and device enumeration.

### Unified Role-Specific Interfaces

In addition to `IVideoDevice`, two narrower interfaces are provided for role-specific access:

| Interface | Header | Purpose |
|---|---|---|
| **VideoInputInterface** | `video_input_interface.h` | Unified API for all capture/receive devices |
| **VideoOutputInterface** | `video_output_interface.h` | Unified API for all playout/send devices |

These interfaces expose only the methods relevant to the device's role (input or output), plus additional features like **timecode access** and **async frame callbacks**.

**Input classes** (DeckLinkInput, NDIInput) implement both `IVideoDevice` and `VideoInputInterface`.
**Output classes** (DeckLinkOutput, NDIOutput) implement both `IVideoDevice` and `VideoOutputInterface`.

## Supported Hardware

### Bidirectional Devices

| Device | Header | Type | SDK Required |
|---|---|---|---|
| **Blackmagic DeckLink** | `decklink_device.h` | SDI/HDMI capture & playout | DeckLink SDK |
| **AJA** | `aja_device.h` | SDI capture & playout | AJA NTV2 SDK |
| **Magewell** | `magewell_device.h` | USB/PCIe capture | Magewell SDK |
| **NDI** | `ndi_device.h` | Network video I/O | NDI SDK |

### Role-Specific Input/Output Classes

| Class | Header | Role | Features |
|---|---|---|---|
| **DeckLinkInput** | `decklink_input.h` | Capture (SDI/HDMI, 1080p/4K) | Connector selection, signal detection, auto-detect mode, low-latency, timecode, frame callback |
| **DeckLinkOutput** | `decklink_output.h` | Playout (SDI/HDMI, 1080p/4K) | Connector selection, genlock reference, active status, low-latency, timecode embed |
| **AJAInput** | `aja_input.h` | Capture | Channel selection, signal detection, auto-detect mode |
| **AJAOutput** | `aja_output.h` | Playout | Channel selection, genlock reference, active status |
| **MagewellInput** | `magewell_input.h` | Capture | Signal detection, auto-detect mode |
| **NDIInput** | `ndi_input.h` | Receive | Network discovery, signal detection, auto-detect mode, NDI HX / Full NDI, timecode, frame callback |
| **NDIOutput** | `ndi_output.h` | Send | Source name configuration, active status, NDI HX / Full NDI, timecode embed |

## Logging

The SDK includes a centralized, thread-safe logging system:

| File | Purpose |
|---|---|
| `sdk_logger.h` | SDKLogger class with log levels (DEBUG, INFO, WARN, ERROR) |
| `sdk_logger.cpp` | Implementation with configurable sinks and mutex guards |

Usage:
```cpp
SDKLogger::setLevel(LogLevel::DEBUG);
SDKLogger::info("DeckLinkInput", "Opened device 0");
SDKLogger::error("NDIOutput", "Failed to send frame");
// Custom sink
SDKLogger::setSink([](LogLevel lvl, const std::string& comp, const std::string& msg) {
    myLogSystem.write(comp, msg);
});
```

## Error Handling

The SDK provides a hierarchy of exception classes for precise error reporting:

| Class | Base | Purpose |
|---|---|---|
| `SDKError` | `std::runtime_error` | Base for all SDK errors |
| `DeckLinkError` | `SDKError` | DeckLink-specific errors (with error code) |
| `NDIError` | `SDKError` | NDI-specific errors (with error code) |
| `DeviceNotFoundError` | `SDKError` | Device enumeration failures |
| `SignalLostError` | `SDKError` | Signal detection failures |

All error classes are defined in `sdk_error.h`.

## SDK Installation

Place vendor SDKs in the respective subdirectories:

```
sdk/
├── decklink/include/   # DeckLink SDK headers
├── aja/include/         # AJA NTV2 SDK headers
├── magewell/include/    # Magewell SDK headers
└── ndi/include/         # NDI SDK headers
```

The CMake build system auto-detects installed SDKs and enables the corresponding device implementations.

See [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) for full details.
