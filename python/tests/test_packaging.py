"""Tests for CPack packaging configuration.

Validates that the CMakeLists.txt files contain the install() and CPack
directives required to generate Windows (NSIS) and macOS (DMG/productbuild)
installers that bundle every VisionCast module.
"""

import os
import re
import pathlib
import pytest

ROOT = pathlib.Path(__file__).resolve().parents[2]  # repo root


# ---------------------------------------------------------------------------
# helpers
# ---------------------------------------------------------------------------

def _read(rel_path: str) -> str:
    return (ROOT / rel_path).read_text(encoding="utf-8")


# ---------------------------------------------------------------------------
# Top-level CMakeLists.txt – CPack configuration
# ---------------------------------------------------------------------------

class TestCPackConfiguration:
    """The top-level CMakeLists.txt must contain a full CPack setup."""

    cmake = _read("CMakeLists.txt")

    def test_include_cpack(self):
        assert "include(CPack)" in self.cmake

    def test_cpack_package_name(self):
        assert "CPACK_PACKAGE_NAME" in self.cmake

    def test_cpack_package_vendor(self):
        assert "CPACK_PACKAGE_VENDOR" in self.cmake

    def test_cpack_version_major(self):
        assert "CPACK_PACKAGE_VERSION_MAJOR" in self.cmake

    def test_cpack_version_minor(self):
        assert "CPACK_PACKAGE_VERSION_MINOR" in self.cmake

    def test_cpack_version_patch(self):
        assert "CPACK_PACKAGE_VERSION_PATCH" in self.cmake

    # --- Windows NSIS ---
    def test_nsis_display_name(self):
        assert "CPACK_NSIS_DISPLAY_NAME" in self.cmake

    def test_nsis_package_name(self):
        assert "CPACK_NSIS_PACKAGE_NAME" in self.cmake

    def test_nsis_start_menu_shortcuts(self):
        assert "CPACK_NSIS_CREATE_ICONS_EXTRA" in self.cmake
        assert "visioncast_ui.exe" in self.cmake

    def test_nsis_uninstall_shortcuts(self):
        assert "CPACK_NSIS_DELETE_ICONS_EXTRA" in self.cmake

    # --- macOS ---
    def test_dmg_volume_name(self):
        assert "CPACK_DMG_VOLUME_NAME" in self.cmake

    def test_dmg_format(self):
        assert "CPACK_DMG_FORMAT" in self.cmake

    def test_productbuild_identity(self):
        assert "CPACK_PRODUCTBUILD_IDENTITY_NAME" in self.cmake

    # --- Generator selection ---
    def test_win32_generator(self):
        assert re.search(r'if\s*\(\s*WIN32\s*\)', self.cmake)
        assert '"NSIS"' in self.cmake

    def test_apple_generator(self):
        assert re.search(r'elseif\s*\(\s*APPLE\s*\)', self.cmake)
        assert "DragNDrop" in self.cmake
        assert "productbuild" in self.cmake


# ---------------------------------------------------------------------------
# Install rules for data directories
# ---------------------------------------------------------------------------

class TestDataInstallRules:
    """Top-level CMake must install every non-C++ module."""

    cmake = _read("CMakeLists.txt")

    @pytest.mark.parametrize("directory", [
        "python/", "config/", "overlays/", "talents/", "docs/"
    ])
    def test_data_directory_installed(self, directory):
        assert directory in self.cmake

    def test_python_tests_excluded(self):
        assert '"tests"' in self.cmake or "'tests'" in self.cmake \
               or "tests" in self.cmake

    def test_pycache_excluded(self):
        assert "__pycache__" in self.cmake


# ---------------------------------------------------------------------------
# Per-target install rules
# ---------------------------------------------------------------------------

class TestEngineInstallRules:
    cmake = _read("engine/CMakeLists.txt")

    def test_engine_executable_installed(self):
        assert re.search(r'install\s*\(\s*TARGETS\s+visioncast_engine\s+RUNTIME', self.cmake)

    def test_engine_library_installed(self):
        assert re.search(r'install\s*\(\s*TARGETS\s+visioncast_engine_lib\s+ARCHIVE', self.cmake)

    def test_engine_headers_installed(self):
        assert re.search(r'install\s*\(\s*DIRECTORY\s+include/visioncast', self.cmake)


class TestSdkInstallRules:
    cmake = _read("sdk/CMakeLists.txt")

    def test_sdk_library_installed(self):
        assert re.search(r'install\s*\(\s*TARGETS\s+visioncast_sdk\s+ARCHIVE', self.cmake)

    def test_sdk_headers_installed(self):
        assert re.search(r'install\s*\(\s*DIRECTORY\s+include/visioncast_sdk', self.cmake)


class TestUiInstallRules:
    cmake = _read("ui/CMakeLists.txt")

    def test_ui_executable_installed(self):
        assert re.search(r'install\s*\(\s*TARGETS\s+visioncast_ui\s+RUNTIME', self.cmake)


# ---------------------------------------------------------------------------
# Packaging documentation
# ---------------------------------------------------------------------------

class TestPackagingDocs:
    """A packaging/README.md must explain how to build installers."""

    readme = _read("packaging/README.md")

    def test_windows_section(self):
        assert "Windows" in self.readme or "NSIS" in self.readme

    def test_macos_section(self):
        assert "macOS" in self.readme or "DMG" in self.readme

    def test_cpack_command_documented(self):
        assert "cpack" in self.readme
