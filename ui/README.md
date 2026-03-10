# UI — Qt Broadcast Control Room

Qt-based operator interface for live broadcast control, monitoring, and
configuration. Two executables are produced from this module:

| Executable | UI Framework | Description |
|---|---|---|
| `visioncast_ui` | Qt Widgets (C++) | Classic dock-based control room |
| `visioncast_ui_qml` | QML / Qt Quick | Modern broadcast-grade QML UI |

---

## Hybrid Architecture

The project uses a **hybrid architecture**: all business logic lives in the
existing C++ classes (`LicenseManager`, `PythonLauncher`, `DeviceScanner`, …),
while a thin `QmlBridge` class bridges them to the QML frontend via
`Q_PROPERTY`, `Q_INVOKABLE`, and signals.

```
ui/
├── src/                    C++ source (backend + entry points)
│   ├── main.cpp            Widgets entry point
│   ├── main_qml.cpp        QML entry point (NEW)
│   ├── qml_bridge.cpp      C++ ↔ QML bridge (NEW)
│   └── …                   Existing panel & service files
├── include/visioncast_ui/
│   ├── qml_bridge.h        QmlBridge class (NEW)
│   └── …                   Existing headers
├── qml/                    QML source tree (NEW)
│   ├── Main.qml            Root ApplicationWindow (1920×1080)
│   ├── panels/             Eight broadcast panels
│   ├── components/         Ten reusable UI components
│   ├── theme/              VCTheme singleton (colors, fonts, spacing)
│   └── qml.qrc             Resource manifest
├── themes/                 QSS stylesheets for the Widgets target
└── resources/              Shared Qt resources (icons)
```

---

## Building

### Qt Widgets target (classic)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target visioncast_ui --parallel
```

### QML target (modern)

Requires Qt Quick and Qt QuickControls2 modules.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target visioncast_ui_qml --parallel
```

### Both targets

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

---

## Qt Widgets Components

| Component | Header | Description |
|---|---|---|
| `MainWindow` | `main_window.h` | Top-level window with dockable panels |
| `PreviewPanel` | `preview_panel.h` | Live video preview + multiview |
| `OverlayEditor` | `overlay_editor.h` | Overlay template editing |
| `TalentManager` | `talent_manager.h` | Talent database CRUD |
| `OutputConfig` | `output_config.h` | Output device/encoder configuration |
| `MonitoringPanel` | `monitoring_panel.h` | System health metrics display |

---

## QML UI Panels

| QML File | Description |
|---|---|
| `panels/SourcePanel.qml` | Camera/source list (left column) |
| `panels/ProgramView.qml` | Program output with LIVE red border |
| `panels/PreviewView.qml` | Preview output with green border |
| `panels/TalentPanel.qml` | Talent database browser |
| `panels/OverlayPanel.qml` | Overlay template browser + toggles |
| `panels/MonitoringPanel.qml` | CPU / GPU / Memory / FPS gauges |
| `panels/OutputPanel.qml` | SDI / NDI / SRT / RTMP config |
| `panels/RecognitionPanel.qml` | AI recognition status |

---

## QML Theme Customisation

All design tokens (colours, fonts, spacing, radii, animation durations) are
centralised in `ui/qml/theme/VCTheme.qml` as a `pragma Singleton`.  No
hardcoded colours appear in individual panel or component files.

To change the accent colour globally, edit `VCTheme.qml`:

```qml
readonly property color accentBlue: "#1F6FEB"   // change here
```

---

## Dependencies

- Qt 5.15+ or Qt 6.x (Widgets, Core, Gui, Network) — for `visioncast_ui`
- Qt 5.15+ or Qt 6.x (Quick, QuickControls2) — additionally for `visioncast_ui_qml`

See [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) for full details.

---

*VisionCast-AI — Licence officielle Prestige Technologie Company,
développée par Glody Dimputu Ngola.*
