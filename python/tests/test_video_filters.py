"""Unit tests for the global video-filter configuration and C++ header/source
consistency.

Validates that:
- config/video_filters.json conforms to the expected schema,
- Every filter declared in the config has a matching C++ class in the header,
- Default parameter ranges are sensible.
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
CONFIG_PATH = os.path.join(PROJECT_ROOT, "config", "video_filters.json")
HEADER_PATH = os.path.join(
    PROJECT_ROOT, "engine", "include", "visioncast", "video_filters.h"
)
SOURCE_PATH = os.path.join(
    PROJECT_ROOT, "engine", "src", "video_filters.cpp"
)

# The five expected filters and their required config keys.
EXPECTED_FILTERS = {
    "cinema_lut": {"enabled", "lut_path", "intensity", "description"},
    "sharpen": {"enabled", "strength", "kernel_size", "description"},
    "noise_reduction": {"enabled", "strength", "sigma_color", "sigma_space", "description"},
    "hdr_tonemap": {"enabled", "gamma", "saturation", "description"},
    "dynamic_contrast": {"enabled", "clip_limit", "tile_grid_size", "description"},
}


# =====================================================================
# Fixture: load config
# =====================================================================

@pytest.fixture(scope="module")
def config():
    assert os.path.isfile(CONFIG_PATH), f"Config not found: {CONFIG_PATH}"
    with open(CONFIG_PATH, "r") as f:
        return json.load(f)


@pytest.fixture(scope="module")
def header_text():
    assert os.path.isfile(HEADER_PATH), f"Header not found: {HEADER_PATH}"
    with open(HEADER_PATH, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def source_text():
    assert os.path.isfile(SOURCE_PATH), f"Source not found: {SOURCE_PATH}"
    with open(SOURCE_PATH, "r") as f:
        return f.read()


# =====================================================================
# Config schema validation
# =====================================================================

class TestVideoFiltersConfig:
    """Validate config/video_filters.json schema and values."""

    def test_config_has_video_filters_key(self, config):
        assert "video_filters" in config

    def test_config_has_filter_order_key(self, config):
        assert "filter_order" in config
        assert isinstance(config["filter_order"], list)

    def test_all_expected_filters_present(self, config):
        filters = config["video_filters"]
        for name in EXPECTED_FILTERS:
            assert name in filters, f"Missing filter: {name}"

    @pytest.mark.parametrize("filter_name", list(EXPECTED_FILTERS.keys()))
    def test_filter_has_required_keys(self, config, filter_name):
        filt = config["video_filters"][filter_name]
        for key in EXPECTED_FILTERS[filter_name]:
            assert key in filt, f"{filter_name} missing key: {key}"

    @pytest.mark.parametrize("filter_name", list(EXPECTED_FILTERS.keys()))
    def test_enabled_is_boolean(self, config, filter_name):
        assert isinstance(config["video_filters"][filter_name]["enabled"], bool)

    @pytest.mark.parametrize("filter_name", list(EXPECTED_FILTERS.keys()))
    def test_description_is_nonempty_string(self, config, filter_name):
        desc = config["video_filters"][filter_name]["description"]
        assert isinstance(desc, str) and len(desc) > 0

    def test_filter_order_contains_all_filters(self, config):
        order_set = set(config["filter_order"])
        expected_set = set(EXPECTED_FILTERS.keys())
        assert order_set == expected_set, (
            f"filter_order mismatch: got {order_set}, expected {expected_set}"
        )

    # --- cinema_lut defaults ---
    def test_cinema_lut_intensity_range(self, config):
        val = config["video_filters"]["cinema_lut"]["intensity"]
        assert 0.0 <= val <= 1.0

    def test_cinema_lut_path_is_string(self, config):
        assert isinstance(config["video_filters"]["cinema_lut"]["lut_path"], str)

    # --- sharpen defaults ---
    def test_sharpen_strength_range(self, config):
        val = config["video_filters"]["sharpen"]["strength"]
        assert 0.0 <= val <= 5.0

    def test_sharpen_kernel_size_is_odd_positive(self, config):
        val = config["video_filters"]["sharpen"]["kernel_size"]
        assert isinstance(val, int) and val >= 1 and val % 2 == 1

    # --- noise_reduction defaults ---
    def test_noise_reduction_strength_range(self, config):
        val = config["video_filters"]["noise_reduction"]["strength"]
        assert 1 <= val <= 50

    def test_noise_reduction_sigma_color_range(self, config):
        val = config["video_filters"]["noise_reduction"]["sigma_color"]
        assert 1.0 <= val <= 300.0

    def test_noise_reduction_sigma_space_range(self, config):
        val = config["video_filters"]["noise_reduction"]["sigma_space"]
        assert 1.0 <= val <= 300.0

    # --- hdr_tonemap defaults ---
    def test_hdr_tonemap_gamma_range(self, config):
        val = config["video_filters"]["hdr_tonemap"]["gamma"]
        assert 0.1 <= val <= 5.0

    def test_hdr_tonemap_saturation_range(self, config):
        val = config["video_filters"]["hdr_tonemap"]["saturation"]
        assert 0.0 <= val <= 3.0

    # --- dynamic_contrast defaults ---
    def test_dynamic_contrast_clip_limit_range(self, config):
        val = config["video_filters"]["dynamic_contrast"]["clip_limit"]
        assert 1.0 <= val <= 40.0

    def test_dynamic_contrast_tile_grid_size_range(self, config):
        val = config["video_filters"]["dynamic_contrast"]["tile_grid_size"]
        assert isinstance(val, int) and 2 <= val <= 32


# =====================================================================
# C++ header/source consistency
# =====================================================================

class TestVideoFiltersHeader:
    """Ensure the C++ header declares all expected filter classes."""

    EXPECTED_CLASSES = [
        "LutFilter",
        "SharpenFilter",
        "NoiseReductionFilter",
        "HdrTonemapFilter",
        "DynamicContrastFilter",
    ]

    @pytest.mark.parametrize("cls_name", EXPECTED_CLASSES)
    def test_class_declared_in_header(self, header_text, cls_name):
        assert cls_name in header_text, (
            f"Class {cls_name} not found in video_filters.h"
        )

    @pytest.mark.parametrize("cls_name", EXPECTED_CLASSES)
    def test_class_inherits_ivideo_filter(self, header_text, cls_name):
        pattern = rf"class\s+{cls_name}\s*:\s*public\s+IVideoFilter"
        assert re.search(pattern, header_text), (
            f"{cls_name} does not inherit from IVideoFilter"
        )

    def test_header_includes_filter_chain(self, header_text):
        assert '#include "visioncast/filter_chain.h"' in header_text

    def test_header_has_pragma_once(self, header_text):
        assert "#pragma once" in header_text


class TestVideoFiltersSource:
    """Ensure the C++ source file implements all expected filters."""

    EXPECTED_NAMES = [
        "cinema_lut",
        "sharpen",
        "noise_reduction",
        "hdr_tonemap",
        "dynamic_contrast",
    ]

    @pytest.mark.parametrize("filter_name", EXPECTED_NAMES)
    def test_name_method_returns_correct_string(self, source_text, filter_name):
        pattern = rf'return\s+"{filter_name}"'
        assert re.search(pattern, source_text), (
            f'name() returning "{filter_name}" not found in video_filters.cpp'
        )

    def test_source_includes_header(self, source_text):
        assert '#include "visioncast/video_filters.h"' in source_text

    def test_source_includes_imgproc(self, source_text):
        assert "opencv2/imgproc.hpp" in source_text


# =====================================================================
# CMakeLists.txt integration
# =====================================================================

class TestCMakeIntegration:
    """Verify the CMake build includes video_filters.cpp."""

    def test_video_filters_in_cmake(self):
        cmake_path = os.path.join(
            PROJECT_ROOT, "engine", "CMakeLists.txt"
        )
        assert os.path.isfile(cmake_path)
        with open(cmake_path, "r") as f:
            content = f.read()
        assert "video_filters.cpp" in content


# =====================================================================
# Config ↔ Header mapping
# =====================================================================

class TestConfigHeaderMapping:
    """Verify that every filter in the config maps to a C++ class."""

    FILTER_CLASS_MAP = {
        "cinema_lut": "LutFilter",
        "sharpen": "SharpenFilter",
        "noise_reduction": "NoiseReductionFilter",
        "hdr_tonemap": "HdrTonemapFilter",
        "dynamic_contrast": "DynamicContrastFilter",
    }

    @pytest.mark.parametrize(
        "config_name,class_name", list(FILTER_CLASS_MAP.items())
    )
    def test_config_filter_has_matching_class(
        self, config, header_text, config_name, class_name
    ):
        assert config_name in config["video_filters"], (
            f"Config missing filter: {config_name}"
        )
        assert class_name in header_text, (
            f"Header missing class: {class_name}"
        )
