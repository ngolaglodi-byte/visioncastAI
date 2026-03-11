"""Unit tests for the broadcast SDK Input/Output C++ header/source consistency.

Validates that:
- Every expected broadcast I/O class has a header and source file,
- Headers declare the correct class inheriting from IVideoDevice,
- Source files include their corresponding headers,
- The CMakeLists.txt includes all new source files.
"""

import os
import re
import sys

import pytest

# Ensure the python/ directory is importable.
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

# Resolve project root (two levels up from tests/).
PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)

SDK_INCLUDE = os.path.join(
    PROJECT_ROOT, "sdk", "include", "visioncast_sdk"
)
SDK_SRC = os.path.join(PROJECT_ROOT, "sdk", "src")
SDK_CMAKE = os.path.join(PROJECT_ROOT, "sdk", "CMakeLists.txt")

# Mapping of class name → (header filename, source filename, DeviceType)
# Note: RTMPOutput and MultiRtmpManager are DEPRECATED and conditionally compiled.
# They require ENABLE_SDK_RTMP=ON during CMake configuration.
# Use visioncast::FFmpegRtmpOutput from engine/include/visioncast/ffmpeg_rtmp.h instead.
BROADCAST_CLASSES = {
    "DeckLinkInput": ("decklink_input.h", "decklink_input.cpp", "CAPTURE"),
    "DeckLinkOutput": ("decklink_output.h", "decklink_output.cpp", "PLAYOUT"),
    "AJAInput": ("aja_input.h", "aja_input.cpp", "CAPTURE"),
    "AJAOutput": ("aja_output.h", "aja_output.cpp", "PLAYOUT"),
    "MagewellInput": ("magewell_input.h", "magewell_input.cpp", "CAPTURE"),
    "NDIInput": ("ndi_input.h", "ndi_input.cpp", "CAPTURE"),
    "NDIOutput": ("ndi_output.h", "ndi_output.cpp", "PLAYOUT"),
    "SRTOutput": ("srt_output.h", "srt_output.cpp", "PLAYOUT"),
    "RTMPOutput": ("rtmp_output.h", "rtmp_output.cpp", "PLAYOUT"),
}

# Classes that are conditionally compiled (deprecated)
CONDITIONAL_CLASSES = {"RTMPOutput"}


# =====================================================================
# Fixture helpers
# =====================================================================

def _read(path):
    assert os.path.isfile(path), f"File not found: {path}"
    with open(path, "r") as f:
        return f.read()


# =====================================================================
# Header file tests
# =====================================================================

class TestBroadcastHeaders:
    """Validate that every broadcast I/O header exists and declares
    the expected class inheriting from IVideoDevice."""

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_header_file_exists(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        path = os.path.join(SDK_INCLUDE, header)
        assert os.path.isfile(path), f"Header not found: {path}"

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_header_has_pragma_once(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "#pragma once" in text

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_header_includes_video_device(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "visioncast_sdk/video_device.h" in text

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_class_inherits_ivideo_device(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        pattern = rf"class\s+{cls_name}\s*:\s*public\s+IVideoDevice"
        assert re.search(pattern, text), (
            f"{cls_name} does not inherit from IVideoDevice in {header}"
        )

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_class_has_open_close(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "open(" in text, f"{cls_name} missing open()"
        assert "close()" in text, f"{cls_name} missing close()"

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_class_has_device_type(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "deviceType()" in text, f"{cls_name} missing deviceType()"

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_class_uses_pimpl(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "struct Impl" in text, f"{cls_name} missing PIMPL idiom"
        assert "std::unique_ptr<Impl>" in text


# =====================================================================
# Source file tests
# =====================================================================

class TestBroadcastSources:
    """Validate that every broadcast I/O source exists and includes
    its header."""

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_source_file_exists(self, cls_name):
        src = BROADCAST_CLASSES[cls_name][1]
        path = os.path.join(SDK_SRC, src)
        assert os.path.isfile(path), f"Source not found: {path}"

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_source_includes_header(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        src = BROADCAST_CLASSES[cls_name][1]
        text = _read(os.path.join(SDK_SRC, src))
        # Header name without extension, used in include path
        include_path = f"visioncast_sdk/{header}"
        assert include_path in text, (
            f"{src} does not include {include_path}"
        )

    @pytest.mark.parametrize("cls_name", list(BROADCAST_CLASSES.keys()))
    def test_source_returns_correct_device_type(self, cls_name):
        src = BROADCAST_CLASSES[cls_name][1]
        expected = BROADCAST_CLASSES[cls_name][2]
        text = _read(os.path.join(SDK_SRC, src))
        pattern = rf"DeviceType::{expected}"
        assert re.search(pattern, text), (
            f"{src} does not return DeviceType::{expected}"
        )


# =====================================================================
# CMakeLists.txt integration
# =====================================================================

class TestCMakeIntegration:
    """Verify sdk/CMakeLists.txt includes all new source files.
    
    Note: Conditional classes (DEPRECATED) are checked separately since they
    are compiled only when ENABLE_SDK_RTMP=ON.
    """

    @pytest.fixture(scope="class")
    def cmake_text(self):
        return _read(SDK_CMAKE)

    @pytest.mark.parametrize("cls_name", [c for c in BROADCAST_CLASSES.keys() if c not in CONDITIONAL_CLASSES])
    def test_source_in_cmake(self, cmake_text, cls_name):
        src = BROADCAST_CLASSES[cls_name][1]
        assert src in cmake_text, (
            f"{src} not found in sdk/CMakeLists.txt"
        )

    @pytest.mark.parametrize("cls_name", list(CONDITIONAL_CLASSES))
    def test_conditional_source_in_cmake(self, cmake_text, cls_name):
        """Conditional (deprecated) sources must be in CMake but may be conditional."""
        src = BROADCAST_CLASSES[cls_name][1]
        # The source should still be mentioned in CMakeLists.txt (within conditional block)
        assert src in cmake_text, (
            f"{src} not found in sdk/CMakeLists.txt (expected in conditional ENABLE_SDK_RTMP block)"
        )


# =====================================================================
# Input-specific tests
# =====================================================================

INPUT_CLASSES = {k: v for k, v in BROADCAST_CLASSES.items() if v[2] == "CAPTURE"}
OUTPUT_CLASSES = {k: v for k, v in BROADCAST_CLASSES.items() if v[2] == "PLAYOUT"}


class TestInputSpecific:
    """Validate that input classes declare signal detection helpers."""

    @pytest.mark.parametrize("cls_name", list(INPUT_CLASSES.keys()))
    def test_input_has_signal_detection(self, cls_name):
        header = INPUT_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "hasSignal()" in text, (
            f"{cls_name} missing hasSignal() method"
        )

    @pytest.mark.parametrize("cls_name", list(INPUT_CLASSES.keys()))
    def test_input_has_detected_mode(self, cls_name):
        header = INPUT_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "detectedMode()" in text, (
            f"{cls_name} missing detectedMode() method"
        )

    @pytest.mark.parametrize("cls_name", list(INPUT_CLASSES.keys()))
    def test_input_playout_noop(self, cls_name):
        src = INPUT_CLASSES[cls_name][1]
        text = _read(os.path.join(SDK_SRC, src))
        assert re.search(r"startPlayout.*return false", text, re.DOTALL), (
            f"{cls_name} startPlayout should return false"
        )


# =====================================================================
# Output-specific tests
# =====================================================================

class TestOutputSpecific:
    """Validate that output classes declare active-status helpers."""

    @pytest.mark.parametrize("cls_name", list(OUTPUT_CLASSES.keys()))
    def test_output_has_active_check(self, cls_name):
        header = OUTPUT_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        assert "isOutputActive()" in text, (
            f"{cls_name} missing isOutputActive() method"
        )

    @pytest.mark.parametrize("cls_name", list(OUTPUT_CLASSES.keys()))
    def test_output_capture_noop(self, cls_name):
        src = OUTPUT_CLASSES[cls_name][1]
        text = _read(os.path.join(SDK_SRC, src))
        assert re.search(r"startCapture.*return false", text, re.DOTALL), (
            f"{cls_name} startCapture should return false"
        )


# =====================================================================
# DeckLink-specific tests (SDI/HDMI, 1080p/4K)
# =====================================================================

class TestDeckLinkConnector:
    """Validate DeckLink classes support SDI/HDMI connector selection and
    1080p/4K video modes."""

    @pytest.mark.parametrize("cls_name", ["DeckLinkInput", "DeckLinkOutput"])
    def test_connector_enum_in_header(self, cls_name):
        header = BROADCAST_CLASSES[cls_name][0]
        text = _read(os.path.join(SDK_INCLUDE, header))
        # At least one of the headers must define or include DeckLinkConnector
        if "DeckLinkConnector" not in text:
            # Check for include that brings in the enum
            other = "decklink_input.h" if cls_name == "DeckLinkOutput" else ""
            if other:
                text2 = _read(os.path.join(SDK_INCLUDE, other))
                assert "DeckLinkConnector" in text2
            else:
                pytest.fail(f"DeckLinkConnector not found for {cls_name}")

    @pytest.mark.parametrize("cls_name", ["DeckLinkInput", "DeckLinkOutput"])
    def test_connector_has_sdi_hdmi(self, cls_name):
        # The enum is defined in decklink_input.h
        text = _read(os.path.join(SDK_INCLUDE, "decklink_input.h"))
        assert "SDI" in text
        assert "HDMI" in text

    @pytest.mark.parametrize("cls_name", ["DeckLinkInput", "DeckLinkOutput"])
    def test_supports_1080p_and_4k(self, cls_name):
        src = BROADCAST_CLASSES[cls_name][1]
        text = _read(os.path.join(SDK_SRC, src))
        assert "1920" in text and "1080" in text, (
            f"{cls_name} missing 1080p support"
        )
        assert "3840" in text and "2160" in text, (
            f"{cls_name} missing 4K support"
        )


# =====================================================================
# NDI-specific tests
# =====================================================================

class TestNDISpecific:
    """Validate NDI-specific features."""

    def test_ndi_input_has_discover_sources(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_input.h"))
        assert "discoverSources()" in text

    def test_ndi_output_has_source_name(self):
        text = _read(os.path.join(SDK_INCLUDE, "ndi_output.h"))
        assert "setSourceName(" in text
        assert "sourceName()" in text


# =====================================================================
# SRT-specific tests
# =====================================================================

class TestSRTSpecific:
    """Validate SRT-specific features."""

    def test_srt_output_has_destination(self):
        text = _read(os.path.join(SDK_INCLUDE, "srt_output.h"))
        assert "setDestination(" in text
        assert "destination()" in text

    def test_srt_output_has_latency(self):
        text = _read(os.path.join(SDK_INCLUDE, "srt_output.h"))
        assert "setLatency(" in text
        assert "latency()" in text

    def test_srt_output_supports_1080p_and_4k(self):
        text = _read(os.path.join(SDK_SRC, "srt_output.cpp"))
        assert "1920" in text and "1080" in text, "SRT missing 1080p support"
        assert "3840" in text and "2160" in text, "SRT missing 4K support"


# =====================================================================
# RTMP-specific tests
# =====================================================================

class TestRTMPSpecific:
    """Validate RTMP-specific features."""

    def test_rtmp_output_has_server_url(self):
        text = _read(os.path.join(SDK_INCLUDE, "rtmp_output.h"))
        assert "setServerUrl(" in text
        assert "serverUrl()" in text

    def test_rtmp_output_has_stream_key(self):
        text = _read(os.path.join(SDK_INCLUDE, "rtmp_output.h"))
        assert "setStreamKey(" in text
        assert "streamKey()" in text

    def test_rtmp_output_supports_1080p_and_4k(self):
        text = _read(os.path.join(SDK_SRC, "rtmp_output.cpp"))
        assert "1920" in text and "1080" in text, "RTMP missing 1080p support"
        assert "3840" in text and "2160" in text, "RTMP missing 4K support"
