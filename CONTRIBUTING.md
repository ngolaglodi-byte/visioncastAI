# Contributing to VisionCast-AI

Thank you for your interest in contributing! Please read these guidelines
before submitting issues or pull requests.

---

## Table of Contents

1. [Development Environment](#development-environment)
2. [Building with vcpkg (recommended)](#building-with-vcpkg-recommended)
3. [Code Conventions](#code-conventions)
4. [Pull Request Process](#pull-request-process)
5. [Running Tests](#running-tests)

---

## Development Environment

### Prerequisites

| Tool | Minimum version |
|------|----------------|
| Python | 3.10 |
| CMake | 3.20 |
| C++ compiler | GCC 11 / Clang 14 / MSVC 2022 (C++17) |
| vcpkg | latest (for C++ dependency management) |

### Python setup

```bash
# Create and activate a virtual environment
python -m venv .venv
source .venv/bin/activate   # Linux / macOS
# .venv\Scripts\activate    # Windows

# Install the package in editable mode with dev extras
pip install -e python/[dev]
```

---

## Building with vcpkg (recommended)

[vcpkg](https://vcpkg.io) is the recommended way to install all C++ dependencies
on any platform. The repository ships a `vcpkg.json` manifest that lists every
required and optional dependency, and a `CMakePresets.json` with ready-to-use
presets for Linux, Windows, and macOS.

### 1. Install vcpkg (one-time setup)

```bash
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
"$HOME/vcpkg/bootstrap-vcpkg.sh"   # Linux / macOS
# On Windows (PowerShell):
# git clone https://github.com/microsoft/vcpkg "C:\vcpkg"
# C:\vcpkg\bootstrap-vcpkg.bat
```

Set the `VCPKG_ROOT` environment variable to your vcpkg directory so that
CMakePresets.json can find the toolchain file:

```bash
# Linux / macOS — add to ~/.bashrc or ~/.zshrc
export VCPKG_ROOT="$HOME/vcpkg"

# Windows (PowerShell profile or System Environment Variables)
$env:VCPKG_ROOT = "C:\vcpkg"
```

### 2. Configure and build

CMakePresets.json provides platform-specific presets that automatically pick up
`VCPKG_ROOT` and enable the vcpkg toolchain.  vcpkg downloads and compiles all
dependencies listed in `vcpkg.json` on the first run (subsequent runs use the
build cache).

```bash
# Linux
cmake --preset linux
cmake --build --preset linux

# macOS
cmake --preset macos
cmake --build --preset macos

# Windows (Developer PowerShell / x64 Native Tools prompt)
cmake --preset windows
cmake --build --preset windows
```

Build outputs land in `build/<preset-name>/`.

### 3. Building without vcpkg (system packages)

You can still build without vcpkg by installing the dependencies via your
system's package manager and running CMake directly:

```bash
# Ubuntu / Debian
sudo apt-get install -y \
  cmake build-essential pkg-config \
  libopencv-dev libzmq3-dev libfreetype6-dev \
  libglfw3-dev libglew-dev \
  libavcodec-dev libavformat-dev libavutil-dev libswscale-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# macOS (Homebrew)
brew install pkg-config opencv zeromq freetype glfw glew ffmpeg

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Windows icon note

The Windows executable icon (`packaging/icons/visioncast.ico`) is a generated
binary asset and is **not** committed to the repository.  The build succeeds
without it — the icon is simply omitted from the `.exe`.  To embed the icon,
generate it from `packaging/icons/visioncast.svg` following the instructions in
`packaging/icons/README.md`.

---

## Code Conventions

### Python

- Follow **PEP 8** (enforced by `flake8`, max line length 99).
- Use **type hints** for all public functions and methods.
- Write **docstrings** for all public classes, methods, and modules
  (Google-style preferred).
- Never use bare `print()` in production code — use the `logging` module.

### C++

- Use **C++17** features where available.
- Follow the existing include-guard style (`#pragma once`).
- Use the `visioncast::` namespace for all public symbols.
- Prefer `std::unique_ptr` / `std::shared_ptr` over raw pointers.
- Document public APIs with Doxygen `/// @brief` comments.

### JSON / YAML / CMake

- 2-space indentation (see `.editorconfig`).
- Trailing whitespace must be trimmed.

---

## Pull Request Process

1. Fork the repository and create a feature branch:
   ```bash
   git checkout -b feat/my-feature
   ```
2. Make your changes, following the code conventions above.
3. Add or update tests to cover your changes.
4. Ensure all CI checks pass locally before pushing:
   ```bash
   # Python lint
   flake8 python/ --max-line-length=99 --extend-ignore=E203,W503

   # Python tests
   cd python && python -m pytest tests/ -v
   ```
5. Open a pull request against the `main` branch with a clear description
   of the changes and the problem they solve.
6. At least one reviewer approval is required before merging.

---

## Running Tests

```bash
# All Python tests (from the python/ directory)
cd python
python -m pytest tests/ -v

# Specific test file
python -m pytest tests/test_gpu_compositor.py -v

# Skip tests that require GPU/dlib
python -m pytest tests/ -v --ignore=tests/test_modules.py
```

Tests are located in `python/tests/`. They use `pytest` and mock
`face_recognition` calls so they run without a GPU or dlib installation.

