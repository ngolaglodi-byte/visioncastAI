"""Unit tests for the VisionCast SDK unified interfaces, logging, and error handling.

Validates that:
- VideoInputInterface and VideoOutputInterface headers exist and declare
  the expected pure virtual methods.
- SDK logging (sdk_logger.h / sdk_logger.cpp) exists with correct API.
- SDK error hierarchy (sdk_error.h) exists with correct class declarations.
- DeckLinkInput / DeckLinkOutput implement the unified interfaces.
- NDIInput / NDIOutput implement the unified interfaces.
- NDI supports bandwidth selection (Full NDI / NDI HX).
- DeckLink supports low-latency mode and timecode.
- Source files use SDKLogger for logging.
- Source files include sdk_error.h for error handling.
- CMakeLists.txt includes the new sdk_logger.cpp source file.
"""

import os
import re
import sys

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)

SDK_INCLUDE = os.path.join(PROJECT_ROOT, "sdk", "include", "visioncast_sdk")
SDK_SRC = os.path.join(PROJECT_ROOT, "sdk", "src")
SDK_CMAKE = os.path.join(PROJECT_ROOT, "sdk", "CMakeLists.txt")


def _read(path):
    assert os.path.isfile(path), f"File not found: {path}"
    with open(path, "r") as f:
        return f.read()


# =====================================================================
# Unified Interfaces
# =====================================================================

class TestVideoInputInterface:
    """Validate VideoInputInterface header."""

    def test_header_exists(self):
        path = os.path.join(SDK_INCLUDE, "video_input_interface.h")
        assert os.path.isfile(path)

    def test_pragma_once(self):
        text = _read(os.path.join(SDK_INCLUDE, "video_input_interface.h"))
        assert "#pragma once" in text

    def test_declares_class(self):
        text = _read(os.path.join(SDK_INCLUDE, "video_input_interface.h"))
        assert re.search(r"class\s+VideoInputInterface", text)

    def test_includes_video_device(self):
        text = _read(os.path.join(SDK_INCLUDE, "video_input_interface.h"))
        assert "visioncast_sdk/video_device.h" in text

    @pytest.mark.parametrize("method", [
        "openInput(", "closeInput()", "isInputOpen()",
        "startCapture(", "stopCapture()", "captureFrame()",
        "setFrameCallback(", "hasSignal()", "detectedMode()",
        "inputDeviceName()", "supportedInputModes()",
        "lastTimecode()",
    ])
    def test_declares_method(self, method):
        text = _read(os.path.join(SDK_INCLUDE, "video_input_interface.h"))
        assert method in text, f"VideoInputInterface missing {method}"


class TestVideoOutputInterface:
    """Validate VideoOutputInterface header."""

    def test_header_exists(self):
        path = os.path.join(SDK_INCLUDE, "video_output_interface.h")
        assert os.path.isfile(path)

    def test_pragma_once(self):
        text = _read(os.path.join(SDK_INCLUDE, "video_output_interface.h"))
        assert "#pragma once" in text

    def test_declares_class(self):
        text = _read(os.path.join(SDK_INCLUDE, "video_output_interface.h"))
        assert re.search(r"class\s+VideoOutputInterface", text)

    def test_includes_video_device(self):
        text = _read(os.path.join(SDK_INCLUDE, "video_output_interface.h"))
        assert "visioncast_sdk/video_device.h" in text

    @pytest.mark.parametrize("method", [
        "openOutput(", "closeOutput()", "isOutputOpen()",
        "startPlayout(", "stopPlayout()", "sendFrame(",
        "isOutputActive()",
        "outputDeviceName()", "supportedOutputModes()",
        "setOutputTimecode(",
    ])
    def test_declares_method(self, method):
        text = _read(os.path.join(SDK_INCLUDE, "video_output_interface.h"))
        assert method in text, f"VideoOutputInterface missing {method}"


# =====================================================================
# SDK Logger
# =====================================================================

class TestSDKLogger:
    """Validate SDK logging utility."""

    def test_header_exists(self):
        assert os.path.isfile(os.path.join(SDK_INCLUDE, "sdk_logger.h"))

    def test_source_exists(self):
        assert os.path.isfile(os.path.join(SDK_SRC, "sdk_logger.cpp"))

    def test_header_declares_class(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_logger.h"))
        assert re.search(r"class\s+SDKLogger", text)

    def test_header_declares_log_level(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_logger.h"))
        assert "LogLevel" in text
        assert "DEBUG" in text
        assert "INFO" in text
        assert "WARN" in text

    @pytest.mark.parametrize("method", [
        "setLevel(", "level()", "setSink(",
        "debug(", "info(", "warn(", "error(",
    ])
    def test_header_declares_method(self, method):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_logger.h"))
        assert method in text, f"SDKLogger missing {method}"

    def test_source_includes_header(self):
        text = _read(os.path.join(SDK_SRC, "sdk_logger.cpp"))
        assert "visioncast_sdk/sdk_logger.h" in text

    def test_cmake_includes_source(self):
        text = _read(SDK_CMAKE)
        assert "sdk_logger.cpp" in text


# =====================================================================
# SDK Error Handling
# =====================================================================

class TestSDKError:
    """Validate SDK error hierarchy."""

    def test_header_exists(self):
        assert os.path.isfile(os.path.join(SDK_INCLUDE, "sdk_error.h"))

    def test_pragma_once(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_error.h"))
        assert "#pragma once" in text

    @pytest.mark.parametrize("cls", [
        "SDKError", "DeckLinkError", "NDIError",
        "DeviceNotFoundError", "SignalLostError",
    ])
    def test_declares_error_class(self, cls):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_error.h"))
        assert re.search(rf"class\s+{cls}", text), f"Missing {cls}"

    def test_sdk_error_inherits_runtime_error(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_error.h"))
        assert re.search(r"class\s+SDKError\s*:\s*public\s+std::runtime_error", text)

    def test_decklink_error_inherits_sdk_error(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_error.h"))
        assert re.search(r"class\s+DeckLinkError\s*:\s*public\s+SDKError", text)

    def test_ndi_error_inherits_sdk_error(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_error.h"))
        assert re.search(r"class\s+NDIError\s*:\s*public\s+SDKError", text)

    def test_has_error_code(self):
        text = _read(os.path.join(SDK_INCLUDE, "sdk_error.h"))
        assert "errorCode()" in text


# =====================================================================
# DeckLink ↔ Unified Interface Integration
# =====================================================================

class TestDeckLinkUnifiedInterface:
    """Validate DeckLinkInput/Output implement the unified interfaces."""

    def test_input_inherits_video_input_interface(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_input.h"))
        assert "VideoInputInterface" in text
        assert re.search(
            r"class\s+DeckLinkInput\s*:.*VideoInputInterface", text
        )

    def test_input_includes_unified_header(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_input.h"))
        assert "video_input_interface.h" in text

    def test_output_inherits_video_output_interface(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_output.h"))
        assert "VideoOutputInterface" in text
        assert re.search(
            r"class\s+DeckLinkOutput\s*:.*VideoOutputInterface", text
        )

    def test_output_includes_unified_header(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_output.h"))
        assert "video_output_interface.h" in text

    def test_input_source_uses_logger(self):
        text = _read(os.path.join(SDK_SRC, "decklink_input.cpp"))
        assert "SDKLogger" in text
        assert "sdk_logger.h" in text

    def test_input_source_uses_error(self):
        text = _read(os.path.join(SDK_SRC, "decklink_input.cpp"))
        assert "sdk_error.h" in text

    def test_output_source_uses_logger(self):
        text = _read(os.path.join(SDK_SRC, "decklink_output.cpp"))
        assert "SDKLogger" in text
        assert "sdk_logger.h" in text

    def test_output_source_uses_error(self):
        text = _read(os.path.join(SDK_SRC, "decklink_output.cpp"))
        assert "sdk_error.h" in text

    def test_input_has_low_latency(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_input.h"))
        assert "setLowLatency(" in text
        assert "lowLatency()" in text

    def test_output_has_low_latency(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_output.h"))
        assert "setLowLatency(" in text
        assert "lowLatency()" in text

    def test_input_has_timecode(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_input.h"))
        assert "lastTimecode()" in text

    def test_output_has_timecode(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_output.h"))
        assert "setOutputTimecode(" in text

    def test_input_has_frame_callback(self):
        text = _read(os.path.join(SDK_INCLUDE, "decklink_input.h"))
        assert "setFrameCallback(" in text


# =====================================================================
# NDI ↔ Unified Interface Integration
# =====================================================================

class TestNDIUnifiedInterface:
    """Validate NDIInput/Output implement the unified interfaces."""

    def test_input_inherits_video_input_interface(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_input.h"))
        assert "VideoInputInterface" in text
        assert re.search(
            r"class\s+NDIInput\s*:.*VideoInputInterface", text
        )

    def test_input_includes_unified_header(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_input.h"))
        assert "video_input_interface.h" in text

    def test_output_inherits_video_output_interface(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_output.h"))
        assert "VideoOutputInterface" in text
        assert re.search(
            r"class\s+NDIOutput\s*:.*VideoOutputInterface", text
        )

    def test_output_includes_unified_header(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_output.h"))
        assert "video_output_interface.h" in text

    def test_input_source_uses_logger(self):
        text = _read(os.path.join(SDK_SRC, "ndi_input.cpp"))
        assert "SDKLogger" in text
        assert "sdk_logger.h" in text

    def test_input_source_uses_error(self):
        text = _read(os.path.join(SDK_SRC, "ndi_input.cpp"))
        assert "sdk_error.h" in text

    def test_output_source_uses_logger(self):
        text = _read(os.path.join(SDK_SRC, "ndi_output.cpp"))
        assert "SDKLogger" in text
        assert "sdk_logger.h" in text

    def test_output_source_uses_error(self):
        text = _read(os.path.join(SDK_SRC, "ndi_output.cpp"))
        assert "sdk_error.h" in text


# =====================================================================
# NDI HX + Full NDI Support
# =====================================================================

class TestNDIBandwidth:
    """Validate NDI bandwidth mode support (Full NDI / NDI HX)."""

    def test_bandwidth_enum_in_header(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_input.h"))
        assert "NDIBandwidth" in text
        assert "FULL" in text
        assert "HX" in text

    def test_input_has_bandwidth_setter(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_input.h"))
        assert "setBandwidth(" in text
        assert "bandwidth()" in text

    def test_output_has_bandwidth_setter(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_output.h"))
        assert "setBandwidth(" in text
        assert "bandwidth()" in text

    def test_input_source_references_hx(self):
        text = _read(os.path.join(SDK_SRC, "ndi_input.cpp"))
        assert "HX" in text

    def test_output_source_references_hx(self):
        text = _read(os.path.join(SDK_SRC, "ndi_output.cpp"))
        assert "HX" in text

    def test_input_has_timecode(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_input.h"))
        assert "lastTimecode()" in text

    def test_output_has_timecode(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_output.h"))
        assert "setOutputTimecode(" in text
