"""Unit tests for the device enumeration bridge.

Validates that:
- device_scanner.h and device_scanner.cpp exist with the expected content,
- output_config.h declares DeviceEntry and the updated OutputSettings,
- output_config.cpp calls refreshDevices() in the constructor and
  uses SDK enumerateDevices() / discoverSources() with compile-time guards,
- main_window.cpp calls scanAllDevices() and passes the result to
  ControlRoom::setSources(),
- ui/CMakeLists.txt includes device_scanner.cpp and links visioncast_sdk.
"""

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)

UI_INCLUDE = os.path.join(PROJECT_ROOT, "ui", "include", "visioncast_ui")
UI_SRC = os.path.join(PROJECT_ROOT, "ui", "src")
UI_CMAKE = os.path.join(PROJECT_ROOT, "ui", "CMakeLists.txt")


def _read(path):
    assert os.path.isfile(path), f"File not found: {path}"
    with open(path) as fh:
        return fh.read()


# =====================================================================
# device_scanner.h
# =====================================================================

class TestDeviceScannerHeader:
    """Validate ui/include/visioncast_ui/device_scanner.h."""

    def test_header_exists(self):
        assert os.path.isfile(os.path.join(UI_INCLUDE, "device_scanner.h"))

    def test_has_pragma_once(self):
        text = _read(os.path.join(UI_INCLUDE, "device_scanner.h"))
        assert "#pragma once" in text

    def test_includes_control_room(self):
        text = _read(os.path.join(UI_INCLUDE, "device_scanner.h"))
        assert "control_room.h" in text

    def test_declares_scan_all_devices(self):
        text = _read(os.path.join(UI_INCLUDE, "device_scanner.h"))
        assert "scanAllDevices()" in text

    def test_returns_vector_of_video_source_info(self):
        text = _read(os.path.join(UI_INCLUDE, "device_scanner.h"))
        assert "VideoSourceInfo" in text

    def test_in_visioncast_ui_namespace(self):
        text = _read(os.path.join(UI_INCLUDE, "device_scanner.h"))
        assert "namespace visioncast_ui" in text


# =====================================================================
# device_scanner.cpp
# =====================================================================

class TestDeviceScannerSource:
    """Validate ui/src/device_scanner.cpp."""

    def test_source_exists(self):
        assert os.path.isfile(os.path.join(UI_SRC, "device_scanner.cpp"))

    def test_includes_own_header(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "device_scanner.h" in text

    def test_includes_decklink(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "decklink_device.h" in text

    def test_includes_aja(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "aja_device.h" in text

    def test_includes_magewell(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "magewell_device.h" in text

    def test_includes_ndi(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "ndi_device.h" in text

    def test_guards_decklink(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "HAS_DECKLINK" in text

    def test_guards_aja(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "HAS_AJA" in text

    def test_guards_magewell(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "HAS_MAGEWELL" in text

    def test_guards_ndi(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "HAS_NDI" in text

    def test_calls_enumerate_devices_decklink(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "DeckLinkDevice::enumerateDevices()" in text

    def test_calls_enumerate_devices_aja(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "AJADevice::enumerateDevices()" in text

    def test_calls_enumerate_devices_magewell(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "MagewellDevice::enumerateDevices()" in text

    def test_calls_discover_sources_ndi(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "NDIDevice::discoverSources()" in text

    def test_adds_virtual_fallback(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "Virtual" in text

    def test_returns_sources_vector(self):
        text = _read(os.path.join(UI_SRC, "device_scanner.cpp"))
        assert "return sources" in text


# =====================================================================
# output_config.h — DeviceEntry struct and updated OutputSettings
# =====================================================================

class TestOutputConfigHeader:
    """Validate ui/include/visioncast_ui/output_config.h changes."""

    def test_header_exists(self):
        assert os.path.isfile(os.path.join(UI_INCLUDE, "output_config.h"))

    def test_declares_device_entry_struct(self):
        text = _read(os.path.join(UI_INCLUDE, "output_config.h"))
        assert "DeviceEntry" in text

    def test_device_entry_has_backend_type(self):
        text = _read(os.path.join(UI_INCLUDE, "output_config.h"))
        assert re.search(r"QString\s+backendType", text)

    def test_device_entry_has_device_index(self):
        text = _read(os.path.join(UI_INCLUDE, "output_config.h"))
        assert re.search(r"int\s+deviceIndex", text)

    def test_output_settings_has_backend_type(self):
        text = _read(os.path.join(UI_INCLUDE, "output_config.h"))
        # OutputSettings struct must also carry backendType
        assert "backendType" in text

    def test_output_settings_has_device_index(self):
        text = _read(os.path.join(UI_INCLUDE, "output_config.h"))
        assert "deviceIndex" in text

    def test_declares_q_declare_metatype(self):
        text = _read(os.path.join(UI_INCLUDE, "output_config.h"))
        assert "Q_DECLARE_METATYPE" in text


# =====================================================================
# output_config.cpp — refreshDevices implementation
# =====================================================================

class TestOutputConfigSource:
    """Validate ui/src/output_config.cpp changes."""

    def test_source_exists(self):
        assert os.path.isfile(os.path.join(UI_SRC, "output_config.cpp"))

    def test_no_hardcoded_combo_items(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        # The old addItems({"DeckLink", ...}) line must be gone
        assert 'addItems({"DeckLink"' not in text

    def test_calls_refresh_devices_in_constructor(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "refreshDevices()" in text

    def test_refresh_devices_clears_combo(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "deviceCombo_->clear()" in text

    def test_includes_decklink_header(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "decklink_device.h" in text

    def test_includes_aja_header(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "aja_device.h" in text

    def test_includes_magewell_header(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "magewell_device.h" in text

    def test_includes_ndi_header(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "ndi_device.h" in text

    def test_guards_decklink(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "HAS_DECKLINK" in text

    def test_guards_aja(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "HAS_AJA" in text

    def test_guards_magewell(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "HAS_MAGEWELL" in text

    def test_guards_ndi(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "HAS_NDI" in text

    def test_adds_virtual_fallback(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "Virtual" in text

    def test_stores_item_data_with_qvariant(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "QVariant" in text or "fromValue" in text

    def test_get_settings_returns_backend_type(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "backendType" in text

    def test_get_settings_returns_device_index(self):
        text = _read(os.path.join(UI_SRC, "output_config.cpp"))
        assert "deviceIndex" in text


# =====================================================================
# MainWindow wiring
# =====================================================================

class TestMainWindowDeviceScannerWiring:
    """Verify MainWindow calls scanAllDevices() and feeds ControlRoom."""

    def test_includes_device_scanner(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "device_scanner.h" in text

    def test_calls_scan_all_devices(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "scanAllDevices()" in text

    def test_passes_to_set_sources(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "setSources" in text


# =====================================================================
# CMakeLists.txt
# =====================================================================

class TestCMakeDeviceScanner:
    """Verify ui/CMakeLists.txt updated correctly."""

    def test_device_scanner_in_sources(self):
        text = _read(UI_CMAKE)
        assert "device_scanner.cpp" in text

    def test_links_visioncast_sdk(self):
        text = _read(UI_CMAKE)
        assert "visioncast_sdk" in text

    def test_includes_sdk_headers(self):
        text = _read(UI_CMAKE)
        assert "sdk/include" in text
