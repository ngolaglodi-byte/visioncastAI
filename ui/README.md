# UI — Qt Broadcast Control Room

Qt-based operator interface for live broadcast control, monitoring, and configuration.

## Components

| Component | Header | Description |
|---|---|---|
| `MainWindow` | `main_window.h` | Top-level window with dockable panels |
| `PreviewPanel` | `preview_panel.h` | Live video preview + multiview |
| `OverlayEditor` | `overlay_editor.h` | Overlay template editing |
| `TalentManager` | `talent_manager.h` | Talent database CRUD |
| `OutputConfig` | `output_config.h` | Output device/encoder configuration |
| `MonitoringPanel` | `monitoring_panel.h` | System health metrics display |

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target visioncast_ui --parallel
```

## Dependencies

- Qt 5.15+ or Qt 6.x (Widgets, Core, Gui)

See [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) for full details.
