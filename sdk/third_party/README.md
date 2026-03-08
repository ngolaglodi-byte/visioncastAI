# sdk/third_party — Proprietary SDK Drop-Zone

This directory is the standard, **non-committed** location for proprietary broadcast
hardware SDKs. Place SDK headers, import libraries, and DLLs here and CMake will
auto-detect them at configure time.

## Usage

Create a sub-folder named after the SDK (see table below) and populate it with the
standard layout:

```
sdk/third_party/
├── decklink/
│   └── include/          # DeckLink SDK headers (DeckLinkAPI.h, etc.)
├── aja/
│   ├── include/          # AJA NTV2 headers (ntv2device.h, etc.)
│   └── lib/              # AJA import libraries (ajantv2.lib / libajantv2.a)
├── magewell/
│   ├── include/          # Magewell LibMWCapture headers (MWCapture.h, etc.)
│   └── lib/              # Magewell import libraries (LibMWCapture.lib, etc.)
├── ndi/
│   ├── include/          # NDI SDK headers (Processing.NDI.Lib.h, etc.)
│   └── lib/
│       └── x64/          # NDI x64 import library (Processing.NDI.Lib.x64.lib)
├── srt/
│   ├── include/          # libsrt headers (srt/srt.h)
│   └── lib/              # libsrt import library (srt.lib / libsrt.a)
└── rtmp/
    └── include/          # Optional RTMP shim headers (leave empty to rely on FFmpeg)
```

## Supported SDK Names

| Folder name | SDK / Vendor | CMake feature flag |
|-------------|--------------|-------------------|
| `decklink`  | Blackmagic Design DeckLink SDK | `HAS_DECKLINK=1` |
| `aja`       | AJA NTV2 SDK | `HAS_AJA=1` |
| `magewell`  | Magewell Pro Capture SDK | `HAS_MAGEWELL=1` |
| `ndi`       | NewTek / Vizrt NDI SDK | `HAS_NDI=1` |
| `srt`       | libsrt (Haivision) | `HAS_SRT=1` |
| `rtmp`      | RTMP shim / FFmpeg | `HAS_RTMP=1` |

## Detection Priority

CMake checks for each SDK in the following order:

1. `sdk/<name>/include/` — legacy / side-by-side layout (still supported)
2. `sdk/third_party/<name>/include/` — this drop-zone (recommended for new setups)

The first path that exists wins. No CMake cache variable needs to be set.

## Git Tracking

The sub-directories `include/`, `lib/`, and `bin/` inside this drop-zone are listed in
`.gitignore` so that proprietary SDK files are **never accidentally committed**.  Only
this `README.md` and the `.gitkeep` marker are tracked by git.

## Obtaining the SDKs

| SDK | Download |
|-----|----------|
| DeckLink | [Blackmagic Design support](https://www.blackmagicdesign.com/support) → *Desktop Video SDK* |
| AJA NTV2 | [AJA Developer Program](https://www.aja.com/developer) |
| Magewell | [Magewell Developer Portal](https://www.magewell.com/sdk) |
| NDI | [NDI SDK](https://ndi.video/for-developers/ndi-sdk/) |
| libsrt | [github.com/Haivision/srt](https://github.com/Haivision/srt/releases) |
| FFmpeg (RTMP/SRT) | [ffmpeg.org/download.html](https://ffmpeg.org/download.html) |
