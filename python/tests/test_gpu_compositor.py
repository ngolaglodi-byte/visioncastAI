"""Unit tests for overlay template validation and compositing configuration.

Validates that overlay template JSON files conform to the expected schema
for the GPU compositing pipeline (animations, transitions, lower thirds,
dynamic templates).
"""

import json
import os
import sys

import pytest

# Ensure the python/ directory is importable.
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

# Resolve project root (two levels up from tests/).
PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)
TEMPLATES_DIR = os.path.join(PROJECT_ROOT, "overlays", "templates")


# =====================================================================
# Template schema validation
# =====================================================================

VALID_ANIMATIONS = {
    "none", "fade_in", "fade_out",
    "slide_left", "slide_right", "slide_up", "slide_down",
    "scale_in", "scale_out",
}

VALID_TRANSITIONS = {
    "none", "cut", "fade",
    "wipe_left", "wipe_right", "wipe_up", "wipe_down",
    "dissolve",
}

VALID_EASINGS = {"linear", "ease_in", "ease_out", "ease_in_out"}


def _load_template(name):
    """Load a template JSON file by name (without extension)."""
    path = os.path.join(TEMPLATES_DIR, f"{name}.json")
    assert os.path.isfile(path), f"Template file not found: {path}"
    with open(path, "r") as f:
        return json.load(f)


class TestOverlayTemplateSchema:
    """Validate that every template in overlays/templates/ has the required
    fields and values expected by the GPU compositor."""

    @pytest.fixture(params=["default", "modern_blue", "minimal"])
    def template_data(self, request):
        return _load_template(request.param)

    def test_has_required_top_level_keys(self, template_data):
        for key in ("name", "description", "style", "layout", "transition"):
            assert key in template_data, f"Missing top-level key: {key}"

    def test_name_is_nonempty_string(self, template_data):
        assert isinstance(template_data["name"], str)
        assert len(template_data["name"]) > 0

    def test_style_has_required_keys(self, template_data):
        style = template_data["style"]
        for key in (
            "background_opacity",
            "font_scale",
            "thickness",
            "band_height_ratio",
            "animation",
            "animation_duration_ms",
            "display_duration_ms",
        ):
            assert key in style, f"Missing style key: {key}"

    def test_background_opacity_in_range(self, template_data):
        val = template_data["style"]["background_opacity"]
        assert 0.0 <= val <= 1.0

    def test_font_scale_positive(self, template_data):
        assert template_data["style"]["font_scale"] > 0

    def test_thickness_positive_integer(self, template_data):
        val = template_data["style"]["thickness"]
        assert isinstance(val, int) and val > 0

    def test_band_height_ratio_in_range(self, template_data):
        val = template_data["style"]["band_height_ratio"]
        assert 0.0 < val < 1.0

    def test_animation_is_valid(self, template_data):
        anim = template_data["style"]["animation"]
        assert anim in VALID_ANIMATIONS, f"Invalid animation: {anim}"

    def test_animation_duration_positive(self, template_data):
        assert template_data["style"]["animation_duration_ms"] > 0

    def test_display_duration_ms_positive(self, template_data):
        style = template_data["style"]
        assert style["display_duration_ms"] > 0

    def test_exit_animation_is_valid(self, template_data):
        style = template_data["style"]
        if "exit_animation" in style:
            assert style["exit_animation"] in VALID_ANIMATIONS

    def test_exit_animation_duration_positive(self, template_data):
        style = template_data["style"]
        if "exit_animation_duration_ms" in style:
            assert style["exit_animation_duration_ms"] > 0

    def test_easing_is_valid(self, template_data):
        style = template_data["style"]
        if "easing" in style:
            assert style["easing"] in VALID_EASINGS

    def test_transition_has_required_keys(self, template_data):
        trans = template_data["transition"]
        for key in ("type", "duration_ms", "easing"):
            assert key in trans, f"Missing transition key: {key}"

    def test_transition_type_is_valid(self, template_data):
        assert template_data["transition"]["type"] in VALID_TRANSITIONS

    def test_transition_duration_positive(self, template_data):
        assert template_data["transition"]["duration_ms"] > 0

    def test_transition_easing_is_valid(self, template_data):
        assert template_data["transition"]["easing"] in VALID_EASINGS

    def test_layout_has_required_keys(self, template_data):
        layout = template_data["layout"]
        required = (
            "title_offset_x",
            "title_offset_y",
            "subtitle_offset_x",
            "subtitle_offset_y",
            "logo_position",
        )
        for key in required:
            assert key in layout, f"Missing layout key: {key}"


# =====================================================================
# Individual template smoke tests
# =====================================================================

class TestDefaultTemplate:
    """Specific checks for the default template."""

    def test_default_template_name(self):
        data = _load_template("default")
        assert data["name"] == "default"

    def test_default_slide_left_animation(self):
        data = _load_template("default")
        assert data["style"]["animation"] == "slide_left"

    def test_default_fade_transition(self):
        data = _load_template("default")
        assert data["transition"]["type"] == "fade"


class TestModernBlueTemplate:
    """Specific checks for the modern_blue template."""

    def test_modern_blue_name(self):
        data = _load_template("modern_blue")
        assert data["name"] == "modern_blue"

    def test_modern_blue_slide_up_animation(self):
        data = _load_template("modern_blue")
        assert data["style"]["animation"] == "slide_up"

    def test_modern_blue_dissolve_transition(self):
        data = _load_template("modern_blue")
        assert data["transition"]["type"] == "dissolve"


class TestMinimalTemplate:
    """Specific checks for the minimal template."""

    def test_minimal_name(self):
        data = _load_template("minimal")
        assert data["name"] == "minimal"

    def test_minimal_fade_in_animation(self):
        data = _load_template("minimal")
        assert data["style"]["animation"] == "fade_in"

    def test_minimal_small_band(self):
        data = _load_template("minimal")
        assert data["style"]["band_height_ratio"] <= 0.12


# =====================================================================
# Template discovery
# =====================================================================

class TestTemplateDiscovery:
    """Ensure the templates directory is well-formed."""

    def test_templates_directory_exists(self):
        assert os.path.isdir(TEMPLATES_DIR)

    def test_at_least_one_template(self):
        templates = [f for f in os.listdir(TEMPLATES_DIR) if f.endswith(".json")]
        assert len(templates) >= 1

    def test_all_templates_are_valid_json(self):
        for fname in os.listdir(TEMPLATES_DIR):
            if not fname.endswith(".json"):
                continue
            path = os.path.join(TEMPLATES_DIR, fname)
            with open(path, "r") as f:
                data = json.load(f)
            assert "name" in data, f"{fname} missing 'name' key"
            assert "style" in data, f"{fname} missing 'style' key"


# =====================================================================
# Lower-third display timer configuration
# =====================================================================

class TestLowerThirdDisplayDuration:
    """Validate display_duration_ms in templates and IPC protocol."""

    @pytest.fixture(params=["default", "modern_blue", "minimal"])
    def template_data(self, request):
        return _load_template(request.param)

    def test_display_duration_is_number(self, template_data):
        val = template_data["style"]["display_duration_ms"]
        assert isinstance(val, (int, float))

    def test_display_duration_reasonable_range(self, template_data):
        val = template_data["style"]["display_duration_ms"]
        assert 1000 <= val <= 60000, "display_duration_ms should be between 1s and 60s"

    def test_default_template_duration(self):
        data = _load_template("default")
        assert data["style"]["display_duration_ms"] == 5000

    def test_modern_blue_template_duration(self):
        data = _load_template("modern_blue")
        assert data["style"]["display_duration_ms"] == 7000

    def test_minimal_template_duration(self):
        data = _load_template("minimal")
        assert data["style"]["display_duration_ms"] == 4000


class TestTalentOverlayDuration:
    """Validate display_duration_ms in the IPC protocol."""

    def test_default_duration(self):
        from ipc.protocol import TalentOverlayMessage
        msg = TalentOverlayMessage(talent_id="t1", name="Alice", role="Host")
        assert msg.display_duration_ms == 5000.0

    def test_custom_duration(self):
        from ipc.protocol import TalentOverlayMessage
        msg = TalentOverlayMessage(
            talent_id="t1", name="Alice", role="Host",
            display_duration_ms=8000.0,
        )
        assert msg.display_duration_ms == 8000.0

    def test_duration_in_json(self):
        from ipc.protocol import TalentOverlayMessage
        msg = TalentOverlayMessage(
            talent_id="t1", name="Alice", role="Host",
            display_duration_ms=3000.0,
        )
        data = json.loads(msg.to_json())
        assert data["display_duration_ms"] == 3000.0

    def test_duration_from_json(self):
        from ipc.protocol import TalentOverlayMessage
        msg = TalentOverlayMessage(
            talent_id="t1", name="Alice", role="Host",
            display_duration_ms=7500.0,
        )
        restored = TalentOverlayMessage.from_json(msg.to_json())
        assert restored.display_duration_ms == 7500.0

    def test_duration_from_json_missing_uses_default(self):
        from ipc.protocol import TalentOverlayMessage
        raw = json.dumps({
            "type": "talent_overlay",
            "talent_id": "t1",
            "name": "Bob",
            "role": "Guest",
        })
        msg = TalentOverlayMessage.from_json(raw)
        assert msg.display_duration_ms == 5000.0
