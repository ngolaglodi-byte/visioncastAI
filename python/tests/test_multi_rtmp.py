"""Unit tests for the MultiFFmpegRtmpManager C++ backend.

Validates that:
- The multi_ffmpeg_rtmp_manager header and source files exist and are consistent.
- The header declares the required public API.
- The source file includes the header and implements key methods.
- The engine CMakeLists.txt compiles the source unconditionally.
- The config/system.json declares rtmp_streams.
- The QML panel file exists and references the bridge properties.

NOTE: This module validates the new engine-based FFmpeg RTMP implementation
(visioncast/multi_ffmpeg_rtmp_manager.h) which replaced the deprecated SDK RTMP
(visioncast_sdk/multi_rtmp_manager.h).
"""

import json
import os
import re

# Resolve project root (two levels up from tests/).
PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)

# Engine module (new FFmpeg-based RTMP implementation)
ENGINE_INCLUDE = os.path.join(PROJECT_ROOT, "engine", "include", "visioncast")
ENGINE_SRC = os.path.join(PROJECT_ROOT, "engine", "src")
ENGINE_CMAKE = os.path.join(PROJECT_ROOT, "engine", "CMakeLists.txt")

UI_INCLUDE = os.path.join(PROJECT_ROOT, "ui", "include", "visioncast_ui")
UI_SRC = os.path.join(PROJECT_ROOT, "ui", "src")
UI_QML = os.path.join(PROJECT_ROOT, "ui", "qml")
CONFIG_FILE = os.path.join(PROJECT_ROOT, "config", "system.json")

MANAGER_HEADER = os.path.join(ENGINE_INCLUDE, "multi_ffmpeg_rtmp_manager.h")
MANAGER_SOURCE = os.path.join(ENGINE_SRC, "multi_ffmpeg_rtmp_manager.cpp")
BRIDGE_HEADER = os.path.join(UI_INCLUDE, "qml_bridge.h")
BRIDGE_SOURCE = os.path.join(UI_SRC, "qml_bridge.cpp")
PANEL_QML = os.path.join(UI_QML, "panels", "MultiStreamPanel.qml")
QML_QRC = os.path.join(UI_QML, "qml.qrc")


def _read(path):
    assert os.path.isfile(path), f"File not found: {path}"
    with open(path, encoding="utf-8") as f:
        return f.read()


# =====================================================================
# Header tests
# =====================================================================

class TestMultiFFmpegRtmpHeader:
    """Validate multi_ffmpeg_rtmp_manager.h content."""

    def test_header_exists(self):
        assert os.path.isfile(MANAGER_HEADER), \
            "multi_ffmpeg_rtmp_manager.h not found"

    def test_header_has_pragma_once(self):
        assert "#pragma once" in _read(MANAGER_HEADER)

    def test_header_includes_ffmpeg_rtmp(self):
        text = _read(MANAGER_HEADER)
        assert "ffmpeg_rtmp.h" in text

    def test_rtmp_status_enum_in_ffmpeg_rtmp(self):
        """Status enum is defined in ffmpeg_rtmp.h, included by manager header."""
        ffmpeg_rtmp_path = os.path.join(ENGINE_INCLUDE, "ffmpeg_rtmp.h")
        text = _read(ffmpeg_rtmp_path)
        assert "RtmpStatus" in text
        assert "Idle" in text
        assert "Connecting" in text
        assert "Live" in text
        assert "Error" in text

    def test_stream_entry_struct_declared(self):
        text = _read(MANAGER_HEADER)
        assert "FFmpegRtmpStreamEntry" in text
        assert "serverUrl" in text
        assert "streamKey" in text
        assert "logLines" in text
        assert "statusMessage" in text

    def test_manager_class_declared(self):
        text = _read(MANAGER_HEADER)
        assert "class MultiFFmpegRtmpManager" in text

    def test_manager_uses_pimpl(self):
        text = _read(MANAGER_HEADER)
        assert "struct Impl" in text
        # Uses shared_ptr for shared ownership across worker threads
        assert "std::shared_ptr<Impl>" in text

    def test_manager_non_copyable(self):
        text = _read(MANAGER_HEADER)
        assert "delete" in text

    def test_manager_has_add_stream(self):
        text = _read(MANAGER_HEADER)
        assert "addStream(" in text

    def test_manager_has_remove_stream(self):
        text = _read(MANAGER_HEADER)
        assert "removeStream(" in text

    def test_manager_has_update_stream(self):
        text = _read(MANAGER_HEADER)
        assert "updateStream(" in text

    def test_manager_has_start_stream(self):
        text = _read(MANAGER_HEADER)
        assert "startStream(" in text

    def test_manager_has_stop_stream(self):
        text = _read(MANAGER_HEADER)
        assert "stopStream(" in text

    def test_manager_has_stop_all(self):
        text = _read(MANAGER_HEADER)
        assert "stopAll()" in text

    def test_manager_has_streams_query(self):
        text = _read(MANAGER_HEADER)
        assert "streams()" in text

    def test_manager_has_get_stream(self):
        text = _read(MANAGER_HEADER)
        assert "getStream(" in text

    def test_manager_has_stream_count(self):
        text = _read(MANAGER_HEADER)
        assert "streamCount()" in text

    def test_manager_has_status_callback(self):
        text = _read(MANAGER_HEADER)
        assert "StatusCallback" in text
        assert "setStatusCallback(" in text

    def test_status_helper_function_in_ffmpeg_rtmp(self):
        """rtmpStatusToString is defined in ffmpeg_rtmp.h."""
        ffmpeg_rtmp_path = os.path.join(ENGINE_INCLUDE, "ffmpeg_rtmp.h")
        text = _read(ffmpeg_rtmp_path)
        assert "rtmpStatusToString(" in text


# =====================================================================
# Source tests
# =====================================================================

class TestMultiFFmpegRtmpSource:
    """Validate multi_ffmpeg_rtmp_manager.cpp content."""

    def test_source_exists(self):
        assert os.path.isfile(MANAGER_SOURCE), \
            "multi_ffmpeg_rtmp_manager.cpp not found"

    def test_source_includes_header(self):
        text = _read(MANAGER_SOURCE)
        assert "visioncast/multi_ffmpeg_rtmp_manager.h" in text

    def test_source_implements_add_stream(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::addStream\(", text)

    def test_source_implements_remove_stream(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::removeStream\(", text)

    def test_source_implements_start_stream(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::startStream\(", text)

    def test_source_implements_stop_stream(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::stopStream\(", text)

    def test_source_implements_stop_all(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::stopAll\(", text)

    def test_source_implements_streams_query(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::streams\(", text)

    def test_source_implements_get_stream(self):
        text = _read(MANAGER_SOURCE)
        assert re.search(r"MultiFFmpegRtmpManager::getStream\(", text)

    def test_source_thread_safe_mutex(self):
        text = _read(MANAGER_SOURCE)
        assert "std::mutex" in text
        assert "std::lock_guard" in text

    def test_source_uses_worker_thread(self):
        text = _read(MANAGER_SOURCE)
        assert "std::thread" in text

    def test_source_uses_atomic_stop_flag(self):
        text = _read(MANAGER_SOURCE)
        assert "std::atomic" in text
        assert "stopRequested" in text

    def test_source_handles_connecting_to_live_transition(self):
        text = _read(MANAGER_SOURCE)
        assert "Connecting" in text
        assert "Live" in text

    def test_max_log_lines_enforced(self):
        text = _read(MANAGER_SOURCE)
        assert "kMaxLogLines" in text


# =====================================================================
# CMake integration
# =====================================================================

class TestCMakeIntegration:
    """Verify engine/CMakeLists.txt compiles multi_ffmpeg_rtmp_manager.cpp."""

    def test_source_in_cmake_engine_sources(self):
        text = _read(ENGINE_CMAKE)
        assert "multi_ffmpeg_rtmp_manager.cpp" in text, \
            "multi_ffmpeg_rtmp_manager.cpp not found in engine/CMakeLists.txt"

    def test_source_in_unconditional_section(self):
        """The source must appear in ENGINE_SOURCES (always compiled)."""
        text = _read(ENGINE_CMAKE)
        # Find the ENGINE_SOURCES block
        match = re.search(r"set\(ENGINE_SOURCES(.*?)\)", text, re.DOTALL)
        assert match, "ENGINE_SOURCES not found in engine/CMakeLists.txt"
        block = match.group(1)
        assert "multi_ffmpeg_rtmp_manager.cpp" in block, \
            "multi_ffmpeg_rtmp_manager.cpp must be in ENGINE_SOURCES"


# =====================================================================
# QML Bridge integration
# =====================================================================

class TestQmlBridgeIntegration:
    """Validate that qml_bridge exposes multi-streaming API."""

    def test_bridge_header_has_rtmp_streams_property(self):
        text = _read(BRIDGE_HEADER)
        assert "rtmpStreams" in text

    def test_bridge_header_has_rtmp_streams_changed_signal(self):
        text = _read(BRIDGE_HEADER)
        assert "rtmpStreamsChanged" in text

    def test_bridge_header_has_rtmp_stream_status_changed_signal(self):
        text = _read(BRIDGE_HEADER)
        assert "rtmpStreamStatusChanged" in text

    def test_bridge_header_has_add_rtmp_stream(self):
        text = _read(BRIDGE_HEADER)
        assert "addRtmpStream(" in text

    def test_bridge_header_has_remove_rtmp_stream(self):
        text = _read(BRIDGE_HEADER)
        assert "removeRtmpStream(" in text

    def test_bridge_header_has_update_rtmp_stream(self):
        text = _read(BRIDGE_HEADER)
        assert "updateRtmpStream(" in text

    def test_bridge_header_has_start_rtmp_stream(self):
        text = _read(BRIDGE_HEADER)
        assert "startRtmpStream(" in text

    def test_bridge_header_has_stop_rtmp_stream(self):
        text = _read(BRIDGE_HEADER)
        assert "stopRtmpStream(" in text

    def test_bridge_header_has_stop_all_rtmp_streams(self):
        text = _read(BRIDGE_HEADER)
        assert "stopAllRtmpStreams()" in text

    def test_bridge_header_has_save_rtmp_config(self):
        text = _read(BRIDGE_HEADER)
        assert "saveRtmpConfig()" in text

    def test_bridge_header_has_rtmp_impl(self):
        text = _read(BRIDGE_HEADER)
        assert "RtmpImpl" in text

    def test_bridge_source_includes_multi_ffmpeg_rtmp_manager(self):
        text = _read(BRIDGE_SOURCE)
        assert "visioncast/multi_ffmpeg_rtmp_manager.h" in text

    def test_bridge_source_implements_add_rtmp_stream(self):
        text = _read(BRIDGE_SOURCE)
        assert re.search(r"QmlBridge::addRtmpStream\(", text)

    def test_bridge_source_implements_start_rtmp_stream(self):
        text = _read(BRIDGE_SOURCE)
        assert re.search(r"QmlBridge::startRtmpStream\(", text)

    def test_bridge_source_implements_refresh_rtmp_streams(self):
        text = _read(BRIDGE_SOURCE)
        assert "refreshRtmpStreams" in text

    def test_bridge_source_populates_default_streams(self):
        text = _read(BRIDGE_SOURCE)
        assert "populateDefaultRtmpStreams" in text


# =====================================================================
# QML Panel
# =====================================================================

class TestMultiStreamPanel:
    """Validate MultiStreamPanel.qml exists and uses bridge API."""

    def test_panel_file_exists(self):
        assert os.path.isfile(PANEL_QML), "MultiStreamPanel.qml not found"

    def test_panel_references_rtmp_streams(self):
        text = _read(PANEL_QML)
        assert "bridge.rtmpStreams" in text

    def test_panel_calls_start_rtmp_stream(self):
        text = _read(PANEL_QML)
        assert "bridge.startRtmpStream" in text

    def test_panel_calls_stop_rtmp_stream(self):
        text = _read(PANEL_QML)
        assert "bridge.stopRtmpStream" in text

    def test_panel_calls_add_rtmp_stream(self):
        text = _read(PANEL_QML)
        assert "bridge.addRtmpStream" in text

    def test_panel_calls_remove_rtmp_stream(self):
        text = _read(PANEL_QML)
        assert "bridge.removeRtmpStream" in text

    def test_panel_calls_stop_all_rtmp_streams(self):
        text = _read(PANEL_QML)
        assert "bridge.stopAllRtmpStreams" in text

    def test_panel_calls_save_rtmp_config(self):
        text = _read(PANEL_QML)
        assert "bridge.saveRtmpConfig" in text

    def test_panel_has_status_indicators(self):
        text = _read(PANEL_QML)
        # Should distinguish live, connecting, error, idle visually.
        assert '"live"' in text or "'live'" in text
        assert '"connecting"' in text or "'connecting'" in text
        assert '"error"' in text or "'error'" in text

    def test_panel_has_platform_support(self):
        text = _read(PANEL_QML)
        assert "youtube" in text
        assert "facebook" in text
        assert "twitch" in text

    def test_panel_has_log_area(self):
        text = _read(PANEL_QML)
        assert "log" in text.lower()

    def test_panel_in_qrc(self):
        text = _read(QML_QRC)
        assert "MultiStreamPanel.qml" in text

    def test_main_qml_uses_multi_stream_panel(self):
        main_qml = _read(os.path.join(UI_QML, "Main.qml"))
        assert "MultiStreamPanel" in main_qml


# =====================================================================
# Config
# =====================================================================

class TestSystemConfig:
    """Validate config/system.json includes rtmp_streams."""

    def test_config_file_exists(self):
        assert os.path.isfile(CONFIG_FILE)

    def test_config_has_rtmp_streams(self):
        with open(CONFIG_FILE, encoding="utf-8") as f:
            data = json.load(f)
        assert "rtmp_streams" in data["output"], \
            "output.rtmp_streams missing from system.json"

    def test_rtmp_streams_is_list(self):
        with open(CONFIG_FILE, encoding="utf-8") as f:
            data = json.load(f)
        assert isinstance(data["output"]["rtmp_streams"], list)

    def test_rtmp_streams_has_required_platforms(self):
        with open(CONFIG_FILE, encoding="utf-8") as f:
            data = json.load(f)
        platforms = {s["platform"] for s in data["output"]["rtmp_streams"]}
        assert "youtube" in platforms
        assert "facebook" in platforms
        assert "twitch" in platforms

    def test_rtmp_stream_entries_have_required_fields(self):
        with open(CONFIG_FILE, encoding="utf-8") as f:
            data = json.load(f)
        for stream in data["output"]["rtmp_streams"]:
            assert "name" in stream
            assert "platform" in stream
            assert "server_url" in stream
            assert "stream_key" in stream
