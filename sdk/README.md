# SDK — Hardware Abstraction Layer

Unified C++ interface for professional video capture and playout hardware.

## Abstract Interface

All devices implement `IVideoDevice` (defined in `include/visioncast_sdk/video_device.h`), providing a common API for capture, playout, and device enumeration.

## Supported Hardware

| Device | Header | Type | SDK Required |
|---|---|---|---|
| **Blackmagic DeckLink** | `decklink_device.h` | SDI/HDMI capture & playout | DeckLink SDK |
| **AJA** | `aja_device.h` | SDI capture & playout | AJA NTV2 SDK |
| **Magewell** | `magewell_device.h` | USB/PCIe capture | Magewell SDK |
| **NDI** | `ndi_device.h` | Network video I/O | NDI SDK |

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
