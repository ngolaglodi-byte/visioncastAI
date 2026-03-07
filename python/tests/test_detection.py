"""Basic smoke tests for the ai.detection module.

Validates that:
- The Detection class can be imported from the ai package.
- The constructor rejects invalid parameters with ValueError.
- Default parameters are applied correctly.
- The module is accessible via the top-level ai package.

These tests use a mocked face_recognition library so they run without
dlib or a GPU being installed.
"""

import sys
from unittest import mock

import pytest

# face_recognition must be present in sys.modules *before* ai.detection is
# imported, because the module-level `try/except ImportError` in detection.py
# resolves at import time.  The fixture in conftest.py uses a context manager
# that only applies within individual test calls, so we set up a lightweight
# mock here at collection time instead.
if "face_recognition" not in sys.modules:
    sys.modules["face_recognition"] = mock.MagicMock()

from ai.detection import Detection  # noqa: E402


# ---------------------------------------------------------------------------
# Parameter validation
# ---------------------------------------------------------------------------

class TestDetectionParameterValidation:
    """Detection constructor must reject invalid parameters."""

    def test_scale_zero_raises_value_error(self):
        with pytest.raises(ValueError, match="scale"):
            Detection(scale=0)

    def test_scale_negative_raises_value_error(self):
        with pytest.raises(ValueError, match="scale"):
            Detection(scale=-0.5)

    def test_scale_above_one_raises_value_error(self):
        with pytest.raises(ValueError, match="scale"):
            Detection(scale=1.1)

    def test_skip_frames_negative_raises_value_error(self):
        with pytest.raises(ValueError, match="skip_frames"):
            Detection(skip_frames=-1)

    def test_valid_scale_boundary_one(self):
        det = Detection(scale=1.0)
        assert det.scale == 1.0

    def test_valid_scale_small(self):
        det = Detection(scale=0.25)
        assert det.scale == 0.25

    def test_skip_frames_zero_valid(self):
        det = Detection(skip_frames=0)
        assert det.skip_frames == 0


# ---------------------------------------------------------------------------
# Default parameter values
# ---------------------------------------------------------------------------

class TestDetectionDefaults:
    """Detection should apply documented default parameter values."""

    def test_default_model_is_hog(self):
        det = Detection()
        assert det.model == "hog"

    def test_default_scale(self):
        det = Detection()
        assert det.scale == 0.5

    def test_default_upsample(self):
        det = Detection()
        assert det.upsample == 1

    def test_default_skip_frames(self):
        det = Detection()
        assert det.skip_frames == 2


# ---------------------------------------------------------------------------
# ai package export
# ---------------------------------------------------------------------------

class TestAiPackageExport:
    """Detection must be exported from the ai package __init__."""

    def test_detection_importable_from_ai(self):
        from ai import Detection as AiDetection
        assert AiDetection is Detection

    def test_detection_in_ai_all(self):
        import ai
        assert "Detection" in ai.__all__
