# VisionCast – Packaging Guide

This document explains how to build **Windows** and **macOS** installers that
bundle every VisionCast module (C++ engine, SDK, Qt UI, Python AI, overlays,
config and talent data).

---

## Prerequisites

| Dependency | Minimum version |
|------------|----------------|
| CMake      | 3.20           |
| C++ compiler (MSVC / Clang / GCC) | C++17 |
| OpenCV     | 4.8            |
| Qt         | 5.15 or 6.x   |
| Python     | 3.11           |
| NSIS       | 3.x *(Windows only)* |

> On macOS the built-in `productbuild` and `hdiutil` tools are used – no extra
> software is required.

---

## Windows Installer (NSIS)

```powershell
# 1. Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 2. Build
cmake --build build --config Release --parallel

# 3. Generate installer
cd build
cpack -G NSIS -C Release
```

The resulting `VisionCast-0.1.0-win64.exe` installer is created in the
`build/` directory. It adds Start-menu shortcuts for **VisionCast UI** and
**VisionCast Engine** and registers an uninstaller.

---

## macOS Installer

### DMG (drag-and-drop disk image)

```bash
# 1. Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 2. Build
cmake --build build --parallel

# 3. Generate DMG
cd build
cpack -G DragNDrop
```

### PKG (productbuild)

```bash
cd build
cpack -G productbuild
```

---

## What Is Included

The installer ships the following components:

| Component | Install path |
|-----------|-------------|
| `visioncast_engine` | `bin/` |
| `visioncast_ui` | `bin/` |
| Engine & SDK static libraries | `lib/` |
| Engine & SDK headers | `include/` |
| Python AI module | `python/` |
| Configuration files | `config/` |
| Overlay templates | `overlays/` |
| Talent database | `talents/` |
| Documentation | `docs/` |

---

## Post-Install: Python Environment

The Python AI module is bundled but still requires a virtual-environment setup
on the target machine:

```bash
cd <install-dir>/python
python -m venv .venv
source .venv/bin/activate   # macOS / Linux
# .venv\Scripts\activate    # Windows
pip install -r requirements.txt
```

---

## Customisation

All CPack variables are set in the top-level `CMakeLists.txt`.  
Override them at configure time, for example:

```bash
cmake -S . -B build \
      -DCPACK_PACKAGE_VERSION_MAJOR=1 \
      -DCPACK_PACKAGE_VERSION_MINOR=0 \
      -DCPACK_PACKAGE_VERSION_PATCH=0
```
