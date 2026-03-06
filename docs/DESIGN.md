# VisionCast Control Room — Design Specification

## Overview

This document describes the complete visual design for the **VisionCast Control Room**, the professional broadcast UI for the VisionCast-AI platform. The design is inspired by industry-standard broadcast tools such as **vMix**, **TriCaster**, **Ross XPression**, and **VizRT**.

## Design Files

| File | Description |
|------|-------------|
| [`docs/mockups/control-room.html`](mockups/control-room.html) | Interactive HTML/CSS mockup — open in any browser |
| [`ui/themes/dark.qss`](../ui/themes/dark.qss) | Qt Style Sheet — Dark theme |
| [`ui/themes/light.qss`](../ui/themes/light.qss) | Qt Style Sheet — Light theme |

## 1. Layout Général

The Control Room uses a **dock-based modular layout** with three main zones:

```
┌─────────────────────────────────────────────────────────┐
│  Top Bar (Menu, Navigation, Live Status, Theme Toggle)  │
├──────────┬──────────────────────────────┬───────────────┤
│          │                              │               │
│ Sidebar  │   Central Zone               │  Right Panel  │
│          │   ┌───────────┬───────────┐  │               │
│ Sources  │   │ PREVIEW   │ PROGRAM   │  │  IA &         │
│ Panels   │   │ (Source)  │ (Output)  │  │  Recognition  │
│ Nav      │   └───────────┴───────────┘  │               │
│          │   ┌─────────────────────────┐│  Logs &       │
│          │   │  Modular Panels (2×N)  ││  Monitoring   │
│          │   │  Sources, Talents,      ││               │
│          │   │  Design, Filters,       ││               │
│          │   │  Duration, etc.         ││               │
│          │   └─────────────────────────┘│               │
├──────────┴──────────────────────────────┴───────────────┤
│  Status Bar                                             │
└─────────────────────────────────────────────────────────┘
```

### Theme System

| Theme | Background | Text | Accent |
|-------|-----------|------|--------|
| **Dark** (default) | `#0d1117` | `#e6edf3` | `#1f6feb` |
| **Light** | `#ffffff` | `#1f2328` | `#0969da` |
| **Broadcast** | Custom per-channel | — | — |

Themes are loaded from `.qss` files in `ui/themes/` and can be switched at runtime via `View > Theme` menu.

## 2. Panneau "Sources Vidéo"

Displays video source information and selection controls.

### Components
- **Source Selector** — Chip-style buttons for DeckLink, AJA, Magewell, NDI
- **Preview Monitors** — Dual monitor layout (Preview / Program)
- **Stats Grid** — Resolution, FPS, Latency in card format
- **Action Buttons** — "Take to PVW" / "Send to PGM"

### Monitor Overlay
- Source label (PREVIEW / PROGRAM)
- Bottom gradient with source name and stats
- Lower-third overlay preview on PROGRAM monitor
- Red border + glow for ON AIR, green border for PREVIEW

## 3. Panneau "Talents & Détails"

Manages talent (on-screen person) database and per-talent configuration.

### Components
- **Talent Cards** — Photo (initials avatar), name, role, organization, overlay tag, animation tag, color dot
- **Detail Form** — Name, Role, Organization, Overlay template selector, Animation IN/OUT selectors, Theme color picker
- **Preview Button** — Triggers overlay preview for selected talent

### Data Model
```json
{
  "talent_id": "talent_1",
  "name": "Marie Dupont",
  "role": "Présentatrice",
  "organization": "France24",
  "photo": "talents/faces/marie_dupont.jpg",
  "overlay": "overlays/templates/modern_blue.json",
  "theme_color": "#1F6FEB",
  "animations": {
    "entry": "fade_in",
    "exit": "fade_out",
    "duration_ms": 500
  }
}
```

## 4. Panneau "Design & Overlays"

Controls the visual design of lower-third overlays and broadcast themes.

### Sub-tabs
1. **Templates** — Grid of template cards with mini-preview (default, modern_blue, minimal, breaking_news)
2. **Thèmes** — Theme swatch selector (Breaking News, News, Sport, Politique, Économie)
3. **Couleurs** — Primary and accent color pickers with hex input
4. **Logos** — Logo file path with browse button, position selector
5. **Transitions** — Animation IN/OUT dropdowns, duration spinner, live transition preview bar

### Template Card Format
```
┌──────────────────┐
│  16:9 Preview    │
│  ▓▓▓▓▓▓▓ (bar)  │
├──────────────────┤
│  Template Name   │
└──────────────────┘
```

## 5. Panneau "Filtres Vidéo"

Video processing filters with per-filter toggle and intensity sliders.

### Filters
| Filter | Type | Default | Presets |
|--------|------|---------|---------|
| **LUTs Cinéma** | Toggle + Slider | OFF | Cinematic Warm, Teal & Orange, Film Log, Rec.709 |
| **Sharpen** | Toggle + Slider | OFF | — |
| **Noise Reduction** | Toggle + Slider | OFF | — |
| **HDR Tonemapping** | Toggle + Slider | OFF | — |
| **Style Broadcast** | Toggle + Presets | OFF | Standard, Vivid, Neutral, Dynamic Contrast |

### Filter Item Layout
```
┌─────────────────────────────────┐
│ 🎬 LUTs Cinéma          [ON/OFF]│
│ [Preset Chips...]               │
│ Intensité ═══════●══════  75%   │
└─────────────────────────────────┘
```

## 6. Panneau "Durée d'Affichage"

Controls how long overlays are displayed on screen.

### Components
- **Timer Display** — Large monospaced number (e.g., "05.0")
- **Duration Input** — Numeric input field (0.5–60 seconds, step 0.5)
- **Mode Toggle** — Automatic / Manual button group
- **Timer Progress** — Animated progress bar showing remaining time

### Modes
- **Automatique** — Overlay auto-hides after the configured duration
- **Manuel** — Overlay stays until manually dismissed

## 7. Panneau "IA & Reconnaissance"

Displays real-time AI face recognition results.

### Components
- **Face Detection Box** — Thumbnail with detection corner markers (green)
- **Recognition Details** — Face detected (✔/✘), Talent name, Role
- **Confidence Bar** — Color-coded progress bar (green >80%, yellow >50%, red <50%)
- **History List** — Timestamped list of recent recognitions with confidence percentage

### Color Coding
| Confidence | Color | Label |
|-----------|-------|-------|
| ≥ 80% | Green (`#2ea043`) | High |
| 50–79% | Yellow (`#d29922`) | Medium |
| < 50% | Red (`#f85149`) | Low |

## 8. Panneau "Logs & Monitoring"

System health monitoring and log viewer.

### Metrics
- **CPU Usage** — Bar + percentage
- **GPU Usage** — Bar + percentage
- **FPS Pipeline** — Bar + value

### Log Tabs
1. **IA** — Face detection/recognition events
2. **Moteur** — Video engine events (overlay applied, frame drops)
3. **ZeroMQ** — IPC message events (PUB/SUB metadata)
4. **Système** — Hardware status (temperature, VRAM, etc.)

### Log Line Format
```
[HH:MM:SS.mmm] [LEVEL] [SOURCE] Message
```

## Color Palette

### Dark Theme
| Role | Hex | Usage |
|------|-----|-------|
| Background Primary | `#0d1117` | Main window background |
| Background Secondary | `#161b22` | Sidebar, menu bar |
| Background Tertiary | `#1c2128` | Panel headers |
| Background Panel | `#21262d` | Panel body, group boxes |
| Border | `#30363d` | Panel borders, separators |
| Text Primary | `#e6edf3` | Main text |
| Text Secondary | `#8b949e` | Labels, hints |
| Accent Blue | `#1f6feb` | Active states, links, focused inputs |
| Success Green | `#2ea043` | Active indicators, high confidence |
| Warning Yellow | `#d29922` | Medium confidence |
| Danger Red | `#f85149` | Errors, low confidence |
| Live Red | `#ff3b3b` | ON AIR indicator |

### Light Theme
| Role | Hex | Usage |
|------|-----|-------|
| Background Primary | `#ffffff` | Main window background |
| Background Secondary | `#f6f8fa` | Sidebar, menu bar |
| Background Tertiary | `#f0f2f5` | Panel headers |
| Border | `#d0d7de` | Panel borders |
| Text Primary | `#1f2328` | Main text |
| Text Secondary | `#656d76` | Labels, hints |
| Accent Blue | `#0969da` | Active states |

## Typography

| Element | Font | Size | Weight |
|---------|------|------|--------|
| Panel Title | System Sans | 11px | 600 (Bold) |
| Body Text | System Sans | 13px | 400 |
| Monospace (logs, stats) | Cascadia Code / JetBrains Mono | 11px | 400 |
| Timer Display | Monospace | 36px | 700 |
| Status Labels | System Sans | 10px | 600, uppercase |

## Spacing & Sizing

| Element | Value |
|---------|-------|
| Border Radius (default) | 6px |
| Border Radius (large) | 10px |
| Panel Padding | 14px |
| Sidebar Width | 240px |
| Top Bar Height | 48px |
| Right Panel Width | 360px |
| Monitor Aspect Ratio | 16:9 |
| Talent Photo | 56×56px (circle) |
