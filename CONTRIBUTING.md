# Contributing to VisionCast-AI

Thank you for your interest in contributing! Please read these guidelines
before submitting issues or pull requests.

---

## Table of Contents

1. [Development Environment](#development-environment)
2. [Code Conventions](#code-conventions)
3. [Pull Request Process](#pull-request-process)
4. [Running Tests](#running-tests)

---

## Development Environment

### Prerequisites

| Tool | Minimum version |
|------|----------------|
| Python | 3.10 |
| CMake | 3.20 |
| C++ compiler | GCC 11 / Clang 14 (C++17) |
| OpenCV | 4.8 |
| ZeroMQ | 4.3 |
| FreeType | 2.13 |

### Python setup

```bash
# Create and activate a virtual environment
python -m venv .venv
source .venv/bin/activate   # Linux / macOS
# .venv\Scripts\activate    # Windows

# Install the package in editable mode with dev extras
pip install -e python/[dev]
```

### C++ engine build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

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
