"""Unit tests for the AI Recognition Panel UI component.

Validates that:
- The recognition_panel header and source files exist,
- The header declares the RecognitionPanel class inheriting from QWidget,
- The header declares the RecognitionEvent struct,
- The source file includes the corresponding header,
- The CMakeLists.txt includes the new source file,
- The MainWindow integrates the RecognitionPanel dock.
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

UI_INCLUDE = os.path.join(PROJECT_ROOT, "ui", "include", "visioncast_ui")
UI_SRC = os.path.join(PROJECT_ROOT, "ui", "src")
UI_CMAKE = os.path.join(PROJECT_ROOT, "ui", "CMakeLists.txt")


def _read(path):
    assert os.path.isfile(path), f"File not found: {path}"
    with open(path, "r") as f:
        return f.read()


# =====================================================================
# Header file tests
# =====================================================================

class TestRecognitionPanelHeader:
    """Validate the recognition_panel.h header."""

    def test_header_exists(self):
        path = os.path.join(UI_INCLUDE, "recognition_panel.h")
        assert os.path.isfile(path)

    def test_header_has_pragma_once(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "#pragma once" in text

    def test_header_includes_qwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "<QWidget>" in text

    def test_header_declares_recognition_event(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "RecognitionEvent" in text

    def test_recognition_event_has_face_name(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert re.search(r"QString\s+faceName", text)

    def test_recognition_event_has_talent_name(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert re.search(r"QString\s+talentName", text)

    def test_recognition_event_has_confidence(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert re.search(r"double\s+confidence", text)

    def test_recognition_event_has_timestamp(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert re.search(r"QDateTime\s+timestamp", text)

    def test_panel_inherits_qwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        pattern = r"class\s+RecognitionPanel\s*:\s*public\s+QWidget"
        assert re.search(pattern, text)

    def test_has_update_recognition(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "updateRecognition(" in text

    def test_has_clear_history(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "clearHistory()" in text

    def test_has_max_history_size(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "maxHistorySize()" in text

    def test_has_recognition_received_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "recognition_panel.h"))
        assert "recognitionReceived(" in text


# =====================================================================
# Source file tests
# =====================================================================

class TestRecognitionPanelSource:
    """Validate the recognition_panel.cpp source."""

    def test_source_exists(self):
        path = os.path.join(UI_SRC, "recognition_panel.cpp")
        assert os.path.isfile(path)

    def test_source_includes_header(self):
        text = _read(os.path.join(UI_SRC, "recognition_panel.cpp"))
        assert "visioncast_ui/recognition_panel.h" in text

    def test_source_has_update_recognition(self):
        text = _read(os.path.join(UI_SRC, "recognition_panel.cpp"))
        assert "RecognitionPanel::updateRecognition" in text

    def test_source_has_clear_history(self):
        text = _read(os.path.join(UI_SRC, "recognition_panel.cpp"))
        assert "RecognitionPanel::clearHistory" in text

    def test_source_has_max_history_size(self):
        text = _read(os.path.join(UI_SRC, "recognition_panel.cpp"))
        assert "RecognitionPanel::maxHistorySize" in text

    def test_source_has_set_max_history_size(self):
        text = _read(os.path.join(UI_SRC, "recognition_panel.cpp"))
        assert "RecognitionPanel::setMaxHistorySize" in text


# =====================================================================
# CMakeLists.txt integration
# =====================================================================

class TestRecognitionPanelCMake:
    """Verify ui/CMakeLists.txt includes the new source file."""

    def test_recognition_panel_in_cmake(self):
        text = _read(UI_CMAKE)
        assert "recognition_panel.cpp" in text


# =====================================================================
# MainWindow integration
# =====================================================================

class TestMainWindowIntegration:
    """Verify the MainWindow integrates the RecognitionPanel dock."""

    def test_main_window_header_has_recognition_panel(self):
        text = _read(os.path.join(UI_INCLUDE, "main_window.h"))
        assert "RecognitionPanel" in text

    def test_main_window_source_includes_recognition_panel(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "recognition_panel.h" in text

    def test_main_window_creates_recognition_panel_dock(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "AI Recognition" in text

    def test_main_window_has_recognition_panel_member(self):
        text = _read(os.path.join(UI_INCLUDE, "main_window.h"))
        assert "recognitionPanel_" in text
