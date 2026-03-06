# Overlays

Overlay graphic templates and assets for broadcast lower-thirds, logos, and tickers.

## Structure

```
overlays/
└── templates/
    ├── default.json       # Default lower-third template (slide-left entry)
    ├── modern_blue.json   # Modern blue-accent template (slide-up entry)
    └── minimal.json       # Minimal template (fade-in entry)
```

## Template Format

Templates are JSON files defining style, layout, animation, and transition parameters for the GPU compositing pipeline.

### Required Keys

| Key | Description |
|---|---|
| `name` | Unique template identifier |
| `description` | Human-readable description |
| `style` | Lower-third styling (opacity, colors, fonts, animations) |
| `layout` | Positioning offsets and logo placement |
| `transition` | Scene transition type, duration, and easing |

### Animations

Entry/exit animations for lower thirds: `fade_in`, `fade_out`, `slide_left`, `slide_right`, `slide_up`, `slide_down`, `scale_in`, `scale_out`.

### Transitions

Scene transitions: `cut`, `fade`, `dissolve`, `wipe_left`, `wipe_right`, `wipe_up`, `wipe_down`.

### Easing Functions

`linear`, `ease_in`, `ease_out`, `ease_in_out`.

See `templates/default.json` for the full schema example.

See [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) for full details.
