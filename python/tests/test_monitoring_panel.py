"""Unit tests for the VisionCast Logs & Monitoring panel.

Validates that:
- The monitoring_panel header and source files exist,
- The header declares the MonitoringPanel class inheriting from QWidget,
- The header declares the SystemMetrics and LogEntry structs,
- The header exposes log methods (appendAiLog, appendEngineLog, appendZmqLog),
- The header exposes clear methods and maxLogLines,
- The source file includes the corresponding header,
- The source file uses QTabWidget with tabs for metrics and logs,
- The CMakeLists.txt includes the source file,
- The MainWindow integrates the MonitoringPanel dock,
- The config/system.json has monitoring configuration,
- The Python IPC protocol defines a LogMessage dataclass.
"""

import json
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

UI_INCLUDE = os.path.join(PROJECT_ROOT, "ui", "include", "visioncast_ui")
UI_SRC = os.path.join(PROJECT_ROOT, "ui", "src")
UI_CMAKE = os.path.join(PROJECT_ROOT, "ui", "CMakeLists.txt")
CONFIG_DIR = os.path.join(PROJECT_ROOT, "config")


def _read(path):
    assert os.path.isfile(path), f"File not found: {path}"
    with open(path, "r") as f:
        return f.read()


# =====================================================================
# Header file tests
# =====================================================================

class TestMonitoringPanelHeader:
    """Validate the monitoring_panel.h header."""

    def test_header_exists(self):
        path = os.path.join(UI_INCLUDE, "monitoring_panel.h")
        assert os.path.isfile(path)

    def test_header_has_pragma_once(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "#pragma once" in text

    def test_header_includes_qwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "<QWidget>" in text

    def test_monitoring_panel_inherits_qwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        pattern = r"class\s+MonitoringPanel\s*:\s*public\s+QWidget"
        assert re.search(pattern, text)

    def test_has_q_object_macro(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "Q_OBJECT" in text

    # -- SystemMetrics struct -----------------------------------------

    def test_declares_system_metrics(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "SystemMetrics" in text

    def test_system_metrics_has_cpu_usage(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "cpuUsage" in text

    def test_system_metrics_has_gpu_usage(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "gpuUsage" in text

    def test_system_metrics_has_fps(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert re.search(r"double\s+fps", text)

    def test_system_metrics_has_latency(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "latencyMs" in text

    def test_system_metrics_has_dropped_frames(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "droppedFrames" in text

    # -- LogEntry struct -----------------------------------------------

    def test_declares_log_entry(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "LogEntry" in text

    def test_log_entry_has_timestamp(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert re.search(r"QDateTime\s+timestamp", text)

    def test_log_entry_has_source(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert re.search(r"QString\s+source", text)

    def test_log_entry_has_level(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert re.search(r"QString\s+level", text)

    def test_log_entry_has_message(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert re.search(r"QString\s+message", text)

    # -- Metrics method ------------------------------------------------

    def test_has_update_metrics(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "updateMetrics(" in text

    # -- Log append methods -------------------------------------------

    def test_has_append_ai_log(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "appendAiLog(" in text

    def test_has_append_engine_log(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "appendEngineLog(" in text

    def test_has_append_zmq_log(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "appendZmqLog(" in text

    # -- Clear methods ------------------------------------------------

    def test_has_clear_ai_logs(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "clearAiLogs()" in text

    def test_has_clear_engine_logs(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "clearEngineLogs()" in text

    def test_has_clear_zmq_logs(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "clearZmqLogs()" in text

    def test_has_clear_all_logs(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "clearAllLogs()" in text

    # -- Max log lines ------------------------------------------------

    def test_has_max_log_lines(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "maxLogLines()" in text

    def test_has_set_max_log_lines(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "setMaxLogLines(" in text

    # -- Tab widget ---------------------------------------------------

    def test_has_tab_widget_member(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "tabWidget_" in text

    # -- Log view members ---------------------------------------------

    def test_has_ai_log_view(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "aiLogView_" in text

    def test_has_engine_log_view(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "engineLogView_" in text

    def test_has_zmq_log_view(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "zmqLogView_" in text

    # -- Forward declarations -----------------------------------------

    def test_forward_declares_qtabwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "QTabWidget" in text

    def test_forward_declares_qplaintextedit(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "QPlainTextEdit" in text

    # -- Private helpers ----------------------------------------------

    def test_has_create_metrics_tab(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "createMetricsTab()" in text

    def test_has_default_max_log_lines_constant(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "kDefaultMaxLogLines" in text

    def test_has_create_log_tab(self):
        text = _read(os.path.join(UI_INCLUDE, "monitoring_panel.h"))
        assert "createLogTab(" in text


# =====================================================================
# Source file tests
# =====================================================================

class TestMonitoringPanelSource:
    """Validate the monitoring_panel.cpp source."""

    def test_source_exists(self):
        path = os.path.join(UI_SRC, "monitoring_panel.cpp")
        assert os.path.isfile(path)

    def test_source_includes_header(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "visioncast_ui/monitoring_panel.h" in text

    def test_source_includes_qtabwidget(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "<QTabWidget>" in text

    def test_source_includes_qplaintextedit(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "<QPlainTextEdit>" in text

    def test_source_has_constructor(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::MonitoringPanel" in text

    def test_source_has_update_metrics(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::updateMetrics" in text

    def test_source_has_append_ai_log(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::appendAiLog" in text

    def test_source_has_append_engine_log(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::appendEngineLog" in text

    def test_source_has_append_zmq_log(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::appendZmqLog" in text

    def test_source_has_clear_ai_logs(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::clearAiLogs" in text

    def test_source_has_clear_engine_logs(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::clearEngineLogs" in text

    def test_source_has_clear_zmq_logs(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::clearZmqLogs" in text

    def test_source_has_clear_all_logs(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::clearAllLogs" in text

    def test_source_has_max_log_lines(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::maxLogLines" in text

    def test_source_has_set_max_log_lines(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::setMaxLogLines" in text

    def test_source_has_create_metrics_tab(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::createMetricsTab" in text

    def test_source_has_create_log_tab(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::createLogTab" in text

    # -- Tab labels ---------------------------------------------------

    def test_source_has_system_metrics_tab(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert '"System Metrics"' in text

    def test_source_has_ai_logs_tab(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert '"AI Logs"' in text

    def test_source_has_engine_logs_tab(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert '"Engine Logs"' in text

    def test_source_has_zmq_logs_tab(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert '"ZeroMQ Logs"' in text

    def test_source_has_append_log_helper(self):
        text = _read(os.path.join(UI_SRC, "monitoring_panel.cpp"))
        assert "MonitoringPanel::appendLog" in text


# =====================================================================
# CMakeLists.txt integration
# =====================================================================

class TestMonitoringPanelCMake:
    """Verify ui/CMakeLists.txt includes the monitoring_panel source file."""

    def test_monitoring_panel_in_cmake(self):
        text = _read(UI_CMAKE)
        assert "monitoring_panel.cpp" in text


# =====================================================================
# MainWindow integration
# =====================================================================

class TestMainWindowIntegration:
    """Verify the MainWindow integrates the MonitoringPanel dock."""

    def test_main_window_header_has_monitoring_panel(self):
        text = _read(os.path.join(UI_INCLUDE, "main_window.h"))
        assert "MonitoringPanel" in text

    def test_main_window_source_includes_monitoring_panel(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "monitoring_panel.h" in text

    def test_main_window_creates_monitoring_panel_dock(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "System Monitor" in text

    def test_main_window_has_monitoring_panel_member(self):
        text = _read(os.path.join(UI_INCLUDE, "main_window.h"))
        assert "monitoringPanel_" in text


# =====================================================================
# Config file validation
# =====================================================================

class TestMonitoringConfig:
    """Validate config/system.json monitoring section."""

    @pytest.fixture()
    def config_data(self):
        path = os.path.join(CONFIG_DIR, "system.json")
        assert os.path.isfile(path), "config/system.json not found"
        with open(path, "r") as f:
            return json.load(f)

    def test_config_has_monitoring_refresh(self, config_data):
        assert "monitoring_refresh_ms" in config_data["ui"]

    def test_monitoring_refresh_is_positive(self, config_data):
        assert config_data["ui"]["monitoring_refresh_ms"] > 0

    def test_config_has_monitoring_section(self, config_data):
        assert "monitoring" in config_data["ui"]

    def test_monitoring_has_max_log_lines(self, config_data):
        m = config_data["ui"]["monitoring"]
        assert "max_log_lines" in m
        assert isinstance(m["max_log_lines"], int)
        assert m["max_log_lines"] > 0

    def test_monitoring_has_log_level(self, config_data):
        m = config_data["ui"]["monitoring"]
        assert "log_level" in m
        assert m["log_level"] in ("DEBUG", "INFO", "WARNING", "ERROR")

    def test_monitoring_has_log_sources(self, config_data):
        m = config_data["ui"]["monitoring"]
        assert "log_sources" in m
        sources = m["log_sources"]
        assert isinstance(sources, list)
        assert "ai" in sources
        assert "engine" in sources
        assert "zmq" in sources


# =====================================================================
# Python IPC protocol — LogMessage
# =====================================================================

class TestLogMessageProtocol:
    """Validate the LogMessage dataclass in python/ipc/protocol.py."""

    def test_log_message_importable(self):
        from ipc.protocol import LogMessage
        assert LogMessage is not None

    def test_log_message_has_source(self):
        from ipc.protocol import LogMessage
        msg = LogMessage(source="ai", message="test")
        assert msg.source == "ai"

    def test_log_message_has_level(self):
        from ipc.protocol import LogMessage
        msg = LogMessage(source="ai", message="test")
        assert msg.level == "INFO"

    def test_log_message_has_message(self):
        from ipc.protocol import LogMessage
        msg = LogMessage(source="ai", message="hello world")
        assert msg.message == "hello world"

    def test_log_message_has_timestamp_ms(self):
        from ipc.protocol import LogMessage
        msg = LogMessage(source="engine", message="test")
        assert hasattr(msg, "timestamp_ms")

    def test_log_message_to_json(self):
        from ipc.protocol import LogMessage
        msg = LogMessage(source="zmq", level="ERROR", message="fail")
        payload = json.loads(msg.to_json())
        assert payload["type"] == "log"
        assert payload["source"] == "zmq"
        assert payload["level"] == "ERROR"
        assert payload["message"] == "fail"
        assert "timestamp_ms" in payload

    def test_log_message_from_json(self):
        from ipc.protocol import LogMessage
        raw = json.dumps({
            "type": "log",
            "source": "ai",
            "level": "WARNING",
            "message": "low confidence",
            "timestamp_ms": 12345,
        })
        msg = LogMessage.from_json(raw)
        assert msg.source == "ai"
        assert msg.level == "WARNING"
        assert msg.message == "low confidence"
        assert msg.timestamp_ms == 12345

    def test_log_message_valid_sources(self):
        from ipc.protocol import LogMessage
        assert "ai" in LogMessage.VALID_SOURCES
        assert "engine" in LogMessage.VALID_SOURCES
        assert "zmq" in LogMessage.VALID_SOURCES

    def test_log_message_valid_levels(self):
        from ipc.protocol import LogMessage
        assert "DEBUG" in LogMessage.VALID_LEVELS
        assert "INFO" in LogMessage.VALID_LEVELS
        assert "WARNING" in LogMessage.VALID_LEVELS
        assert "ERROR" in LogMessage.VALID_LEVELS

    def test_log_message_rejects_invalid_source(self):
        from ipc.protocol import LogMessage
        with pytest.raises(ValueError, match="Invalid source"):
            LogMessage(source="unknown", message="test")

    def test_log_message_rejects_invalid_level(self):
        from ipc.protocol import LogMessage
        with pytest.raises(ValueError, match="Invalid level"):
            LogMessage(source="ai", level="CRITICAL", message="test")
