"""Unit tests for the talent database schema used by the talent detail panel.

Validates that ``talents/talents.json`` conforms to the expected structure
so the UI TalentManager panel can manage every talent entry properly
(name, role, organisation, photo, overlay, colours, animations).
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
TALENTS_JSON = os.path.join(PROJECT_ROOT, "talents", "talents.json")

VALID_ANIMATIONS = {
    "none", "fade_in", "fade_out",
    "slide_left", "slide_right", "slide_up", "slide_down",
    "scale_in", "scale_out",
}


# =====================================================================
# Helpers
# =====================================================================

_cached_talents = None


def _load_talents():
    """Load and return the list of talent dicts from talents.json (cached)."""
    global _cached_talents
    if _cached_talents is not None:
        return _cached_talents
    assert os.path.isfile(TALENTS_JSON), f"Missing {TALENTS_JSON}"
    with open(TALENTS_JSON, "r", encoding="utf-8") as f:
        data = json.load(f)
    assert "talents" in data, "talents.json must have a 'talents' key"
    _cached_talents = data["talents"]
    return _cached_talents


@pytest.fixture(params=range(len(_load_talents())))
def talent(request):
    """Yield each talent entry from the database, one per test invocation."""
    return _load_talents()[request.param]


# =====================================================================
# Database-level tests
# =====================================================================

class TestTalentDatabase:
    """Ensure the talent database file is well-formed."""

    def test_file_exists(self):
        assert os.path.isfile(TALENTS_JSON)

    def test_valid_json(self):
        with open(TALENTS_JSON, "r", encoding="utf-8") as f:
            data = json.load(f)
        assert isinstance(data, dict)

    def test_has_talents_array(self):
        with open(TALENTS_JSON, "r", encoding="utf-8") as f:
            data = json.load(f)
        assert "talents" in data
        assert isinstance(data["talents"], list)

    def test_at_least_one_talent(self):
        talents = _load_talents()
        assert len(talents) >= 1


# =====================================================================
# Per-talent schema validation
# =====================================================================

class TestTalentSchema:
    """Validate every talent entry has the fields required by the panel."""

    # --- required top-level keys ---

    def test_has_name(self, talent):
        assert "name" in talent
        assert isinstance(talent["name"], str) and len(talent["name"]) > 0

    def test_has_role(self, talent):
        assert "role" in talent
        assert isinstance(talent["role"], str) and len(talent["role"]) > 0

    def test_has_organization(self, talent):
        assert "organization" in talent
        assert isinstance(talent["organization"], str)

    def test_has_photo(self, talent):
        assert "photo" in talent
        assert isinstance(talent["photo"], str) and len(talent["photo"]) > 0

    def test_photo_path_exists(self, talent):
        photo = os.path.join(PROJECT_ROOT, talent["photo"])
        assert os.path.isfile(photo), f"Photo not found: {photo}"

    def test_has_overlay(self, talent):
        assert "overlay" in talent
        assert isinstance(talent["overlay"], str) and len(talent["overlay"]) > 0

    def test_overlay_path_exists(self, talent):
        overlay = os.path.join(PROJECT_ROOT, talent["overlay"])
        assert os.path.isfile(overlay), f"Overlay template not found: {overlay}"

    # --- colour ---

    def test_has_theme_color(self, talent):
        assert "theme_color" in talent
        color = talent["theme_color"]
        assert isinstance(color, str) and color.startswith("#")
        assert len(color) in (4, 7), "Expected #RGB or #RRGGBB"

    # --- animations ---

    def test_has_animations(self, talent):
        assert "animations" in talent
        assert isinstance(talent["animations"], dict)

    def test_animation_entry_is_valid(self, talent):
        entry = talent["animations"].get("entry")
        assert entry in VALID_ANIMATIONS, f"Invalid entry animation: {entry}"

    def test_animation_exit_is_valid(self, talent):
        exit_anim = talent["animations"].get("exit")
        assert exit_anim in VALID_ANIMATIONS, f"Invalid exit animation: {exit_anim}"

    def test_animation_duration_positive(self, talent):
        dur = talent["animations"].get("duration_ms")
        assert isinstance(dur, (int, float)) and dur > 0


# =====================================================================
# Cross-checks with overlay templates
# =====================================================================

class TestTalentOverlayCrossRef:
    """Ensure overlay references in the talent DB point to valid templates."""

    def test_overlay_is_valid_json(self, talent):
        overlay_path = os.path.join(PROJECT_ROOT, talent["overlay"])
        with open(overlay_path, "r", encoding="utf-8") as f:
            data = json.load(f)
        assert "name" in data
        assert "style" in data
