"""Tests for the robustness improvements in main.py.

Validates that:
- load_talents() handles missing / malformed JSON gracefully.
- load_talent_encodings() skips talents with missing or bad photos.
- run() never calls sys.exit and returns gracefully on camera failure.
"""

import json
import os
import sys
import tempfile
from unittest import mock

import numpy as np
import pytest

# Ensure the python/ directory is importable.
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))


# ---------------------------------------------------------------------------
# We must mock heavy native dependencies *before* importing main so that
# module-level code does not crash in CI (no camera, no dlib).
# The mocks are installed permanently in sys.modules so that @mock.patch
# decorators can resolve the ``main`` module later.
# ---------------------------------------------------------------------------

_mock_fr = mock.MagicMock(name="face_recognition")
_mock_fr.load_image_file.return_value = np.zeros((100, 100, 3), dtype=np.uint8)
_mock_fr.face_encodings.return_value = [np.random.rand(128)]

_mock_cv2 = mock.MagicMock(name="cv2")

sys.modules.setdefault("face_recognition", _mock_fr)
sys.modules.setdefault("cv2", _mock_cv2)

from main import load_talents, load_talent_encodings, run  # noqa: E402


# ======================================================================
# load_talents tests
# ======================================================================

class TestLoadTalents:

    def test_valid_file(self, tmp_path):
        data = {"talents": [{"name": "Alice"}, {"name": "Bob"}]}
        path = tmp_path / "talents.json"
        path.write_text(json.dumps(data), encoding="utf-8")

        result = load_talents(str(path))
        assert len(result) == 2
        assert result[0]["name"] == "Alice"

    def test_missing_file(self, tmp_path):
        result = load_talents(str(tmp_path / "nonexistent.json"))
        assert result == []

    def test_malformed_json(self, tmp_path):
        path = tmp_path / "bad.json"
        path.write_text("{invalid json", encoding="utf-8")

        result = load_talents(str(path))
        assert result == []

    def test_missing_talents_key(self, tmp_path):
        path = tmp_path / "no_key.json"
        path.write_text(json.dumps({"other": []}), encoding="utf-8")

        result = load_talents(str(path))
        assert result == []


# ======================================================================
# load_talent_encodings tests
# ======================================================================

class TestLoadTalentEncodings:

    @mock.patch("main.face_recognition")
    def test_successful_load(self, mock_fr):
        fake_enc = np.random.rand(128)
        mock_fr.load_image_file.return_value = np.zeros((100, 100, 3))
        mock_fr.face_encodings.return_value = [fake_enc]

        talents = [{"name": "Alice", "photo": "img/alice.jpg"}]
        encodings, metadata = load_talent_encodings(talents, "/fake/root")

        assert len(encodings) == 1
        assert len(metadata) == 1
        assert metadata[0]["name"] == "Alice"

    @mock.patch("main.face_recognition")
    def test_missing_photo_skipped(self, mock_fr):
        mock_fr.load_image_file.side_effect = FileNotFoundError("not found")

        talents = [{"name": "Ghost", "photo": "img/ghost.jpg"}]
        encodings, metadata = load_talent_encodings(talents, "/fake/root")

        assert encodings == []
        assert metadata == []

    @mock.patch("main.face_recognition")
    def test_encoding_failure_skipped(self, mock_fr):
        mock_fr.load_image_file.return_value = np.zeros((100, 100, 3))
        mock_fr.face_encodings.side_effect = RuntimeError("dlib exploded")

        talents = [{"name": "Bad", "photo": "img/bad.jpg"}]
        encodings, metadata = load_talent_encodings(talents, "/fake/root")

        assert encodings == []
        assert metadata == []

    @mock.patch("main.face_recognition")
    def test_no_face_in_photo_skipped(self, mock_fr):
        mock_fr.load_image_file.return_value = np.zeros((100, 100, 3))
        mock_fr.face_encodings.return_value = []  # no face found

        talents = [{"name": "NoFace", "photo": "img/noface.jpg"}]
        encodings, metadata = load_talent_encodings(talents, "/fake/root")

        assert encodings == []
        assert metadata == []

    @mock.patch("main.face_recognition")
    def test_partial_load_with_fallback(self, mock_fr):
        """When one talent fails, the rest should still load."""
        good_enc = np.random.rand(128)

        def _load_image(path):
            if "missing" in path:
                raise FileNotFoundError("nope")
            return np.zeros((100, 100, 3))

        mock_fr.load_image_file.side_effect = _load_image
        mock_fr.face_encodings.return_value = [good_enc]

        talents = [
            {"name": "Missing", "photo": "img/missing.jpg"},
            {"name": "Good", "photo": "img/good.jpg"},
        ]
        encodings, metadata = load_talent_encodings(talents, "/fake/root")

        assert len(encodings) == 1
        assert metadata[0]["name"] == "Good"


# ======================================================================
# run() tests
# ======================================================================

class TestRun:

    @mock.patch("main.cv2")
    @mock.patch("main.face_recognition")
    def test_no_sys_exit_on_camera_failure(self, mock_fr, mock_cv2):
        """run() must never call sys.exit; it should return gracefully."""
        mock_cap = mock.MagicMock()
        mock_cap.isOpened.return_value = False
        mock_cv2.VideoCapture.return_value = mock_cap

        # Should return without raising or calling sys.exit.
        run()

        mock_cap.release.assert_called_once()

    @mock.patch("main.cv2")
    @mock.patch("main.face_recognition")
    def test_videocapture_exception_handled(self, mock_fr, mock_cv2):
        """If VideoCapture() itself throws, run() should return gracefully."""
        mock_cv2.VideoCapture.side_effect = RuntimeError("device error")

        run()  # should not raise

    @mock.patch("main.time")
    @mock.patch("main.cv2")
    @mock.patch("main.face_recognition")
    def test_frame_read_retries_then_stops(self, mock_fr, mock_cv2, mock_time):
        """Consecutive frame-read failures should cause a controlled stop."""
        mock_cap = mock.MagicMock()
        mock_cap.isOpened.return_value = True
        mock_cap.read.return_value = (False, None)
        mock_cv2.VideoCapture.return_value = mock_cap

        run()

        # The implementation retries up to MAX_READ_FAILURES (30) times.
        assert mock_cap.read.call_count >= 30
        mock_cap.release.assert_called_once()
