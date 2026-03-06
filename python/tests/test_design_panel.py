"""Unit tests for the VisionCast Design & Overlays panel.

Validates that:
- The design_panel header and source files exist,
- The header declares the DesignPanel class inheriting from QWidget,
- The header exposes sub-sections: lower thirds, templates, themes,
  colours, logos, and transitions,
- The source file includes the corresponding header,
- The CMakeLists.txt includes the new source file,
- The MainWindow integrates the DesignPanel dock,
- The config/design.json configuration file is well-formed.
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

class TestDesignPanelHeader:
    """Validate the design_panel.h header."""

    def test_header_exists(self):
        path = os.path.join(UI_INCLUDE, "design_panel.h")
        assert os.path.isfile(path)

    def test_header_has_pragma_once(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "#pragma once" in text

    def test_header_includes_qwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "<QWidget>" in text

    def test_design_panel_inherits_qwidget(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        pattern = r"class\s+DesignPanel\s*:\s*public\s+QWidget"
        assert re.search(pattern, text)

    def test_has_q_object_macro(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "Q_OBJECT" in text

    # -- Lower thirds -------------------------------------------------

    def test_has_set_lower_third_title(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setLowerThirdTitle(" in text

    def test_has_set_lower_third_subtitle(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setLowerThirdSubtitle(" in text

    def test_has_lower_third_title_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "lowerThirdTitle()" in text

    def test_has_lower_third_subtitle_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "lowerThirdSubtitle()" in text

    def test_has_lower_third_applied_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "lowerThirdApplied(" in text

    # -- Templates ----------------------------------------------------

    def test_has_current_template_name(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "currentTemplateName()" in text

    # -- Themes -------------------------------------------------------

    def test_has_set_theme(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setTheme(" in text

    def test_has_current_theme(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "currentTheme()" in text

    def test_has_theme_changed_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "themeChanged(" in text

    # -- Colours ------------------------------------------------------

    def test_has_set_primary_color(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setPrimaryColor(" in text

    def test_has_set_accent_color(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setAccentColor(" in text

    def test_has_primary_color_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "primaryColor()" in text

    def test_has_accent_color_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "accentColor()" in text

    def test_has_colors_changed_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "colorsChanged(" in text

    # -- Logo ---------------------------------------------------------

    def test_has_set_logo_path(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setLogoPath(" in text

    def test_has_logo_path_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "logoPath()" in text

    def test_has_logo_changed_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "logoChanged(" in text

    # -- Transitions --------------------------------------------------

    def test_has_set_transition_type(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setTransitionType(" in text

    def test_has_set_transition_duration(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "setTransitionDuration(" in text

    def test_has_transition_type_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "transitionType()" in text

    def test_has_transition_duration_getter(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "transitionDuration()" in text

    def test_has_transition_changed_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "transitionChanged(" in text

    # -- Config -------------------------------------------------------

    def test_has_load_config(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "loadConfig(" in text

    def test_has_save_config(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "saveConfig(" in text

    # -- General signal -----------------------------------------------

    def test_has_design_changed_signal(self):
        text = _read(os.path.join(UI_INCLUDE, "design_panel.h"))
        assert "designChanged()" in text


# =====================================================================
# Source file tests
# =====================================================================

class TestDesignPanelSource:
    """Validate the design_panel.cpp source."""

    def test_source_exists(self):
        path = os.path.join(UI_SRC, "design_panel.cpp")
        assert os.path.isfile(path)

    def test_source_includes_header(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "visioncast_ui/design_panel.h" in text

    def test_source_has_constructor(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::DesignPanel" in text

    def test_source_has_set_lower_third_title(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setLowerThirdTitle" in text

    def test_source_has_set_lower_third_subtitle(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setLowerThirdSubtitle" in text

    def test_source_has_current_template_name(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::currentTemplateName" in text

    def test_source_has_set_theme(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setTheme" in text

    def test_source_has_set_primary_color(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setPrimaryColor" in text

    def test_source_has_set_accent_color(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setAccentColor" in text

    def test_source_has_set_logo_path(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setLogoPath" in text

    def test_source_has_set_transition_type(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setTransitionType" in text

    def test_source_has_set_transition_duration(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::setTransitionDuration" in text

    def test_source_has_load_config(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::loadConfig" in text

    def test_source_has_save_config(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "DesignPanel::saveConfig" in text

    def test_source_creates_lower_thirds_tab(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "createLowerThirdsTab" in text

    def test_source_creates_templates_tab(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "createTemplatesTab" in text

    def test_source_creates_themes_tab(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "createThemesTab" in text

    def test_source_creates_colors_tab(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "createColorsTab" in text

    def test_source_creates_logos_tab(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "createLogosTab" in text

    def test_source_creates_transitions_tab(self):
        text = _read(os.path.join(UI_SRC, "design_panel.cpp"))
        assert "createTransitionsTab" in text


# =====================================================================
# CMakeLists.txt integration
# =====================================================================

class TestDesignPanelCMake:
    """Verify ui/CMakeLists.txt includes the new source file."""

    def test_design_panel_in_cmake(self):
        text = _read(UI_CMAKE)
        assert "design_panel.cpp" in text


# =====================================================================
# MainWindow integration
# =====================================================================

class TestMainWindowDesignIntegration:
    """Verify the MainWindow integrates the DesignPanel."""

    def test_main_window_header_has_design_panel(self):
        text = _read(os.path.join(UI_INCLUDE, "main_window.h"))
        assert "DesignPanel" in text

    def test_main_window_source_includes_design_panel(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "design_panel.h" in text

    def test_main_window_creates_design_panel_dock(self):
        text = _read(os.path.join(UI_SRC, "main_window.cpp"))
        assert "Design & Overlays" in text


# =====================================================================
# Config file validation
# =====================================================================

class TestDesignConfig:
    """Validate config/design.json structure and defaults."""

    @pytest.fixture()
    def config_data(self):
        path = os.path.join(CONFIG_DIR, "design.json")
        assert os.path.isfile(path), "config/design.json not found"
        with open(path, "r") as f:
            return json.load(f)

    def test_config_file_exists(self):
        assert os.path.isfile(os.path.join(CONFIG_DIR, "design.json"))

    def test_has_lower_thirds_section(self, config_data):
        assert "lower_thirds" in config_data

    def test_has_templates_section(self, config_data):
        assert "templates" in config_data

    def test_has_themes_section(self, config_data):
        assert "themes" in config_data

    def test_has_colors_section(self, config_data):
        assert "colors" in config_data

    def test_has_logo_section(self, config_data):
        assert "logo" in config_data

    def test_has_transitions_section(self, config_data):
        assert "transitions" in config_data

    def test_templates_has_available_list(self, config_data):
        assert isinstance(config_data["templates"]["available"], list)
        assert len(config_data["templates"]["available"]) >= 1

    def test_themes_has_available_list(self, config_data):
        assert isinstance(config_data["themes"]["available"], list)
        assert len(config_data["themes"]["available"]) >= 1

    def test_colors_has_primary_and_accent(self, config_data):
        colors = config_data["colors"]
        assert "primary" in colors
        assert "accent" in colors

    def test_transitions_has_type_and_duration(self, config_data):
        trans = config_data["transitions"]
        assert "type" in trans
        assert "duration_ms" in trans
        assert trans["duration_ms"] > 0

    def test_logo_has_path_and_position(self, config_data):
        logo = config_data["logo"]
        assert "path" in logo
        assert "position" in logo
