# Engine — C++ Video Processing Engine

Real-time 4K video processing and compositing engine for broadcast production.

## Architecture

| Component | Header | Description |
|---|---|---|
| `CaptureManager` | `capture_manager.h` | Manages video input sources (SDI, HDMI, NDI) |
| `PipelineManager` | `pipeline_manager.h` | Orchestrates capture → filter → overlay → output |
| `OverlayRenderer` | `overlay_renderer.h` | Composites lower-thirds, logos, tickers |
| `FilterChain` | `filter_chain.h` | Sequential video filter pipeline |
| `OutputManager` | `output_manager.h` | Routes frames to broadcast outputs |
| `MetadataReceiver` | `metadata_receiver.h` | Receives AI metadata from Python via ZeroMQ |

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target visioncast_engine --parallel
```

## Dependencies

- OpenCV ≥ 4.8
- FFmpeg ≥ 6.0
- ZeroMQ ≥ 4.3
- nlohmann/json
- FreeType ≥ 2.13

See [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) for full details.
