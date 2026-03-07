"""Unit tests for the optimized face matching in recognition.py and face_matcher.py.

Validates:
- face_distance is used (not compare_faces + index(True))
- Minimum distance selection returns the closest talent
- Configurable threshold (tolerance) is respected
- Confidence logs are emitted via the logging module
- Edge cases (empty database, no encodings)

All face_recognition calls are mocked so tests run without dlib/GPU.
"""

import logging
import sys
from unittest import mock

import numpy as np
import pytest

# Ensure face_recognition mock is available before module import.
if "face_recognition" not in sys.modules:
    sys.modules["face_recognition"] = mock.MagicMock()


# =====================================================================
# recognition.py – _match_all tests
# =====================================================================

class TestMatchAll:
    """Tests for Recognition._match_all optimized matching."""

    def _make_pipeline(self, known_encodings, talents, tolerance=0.45):
        """Helper: build a Recognition instance with mocked internals."""
        from ai.recognition import Recognition

        loader = mock.MagicMock()
        loader.encodings = known_encodings
        loader.get_talent = lambda idx: talents[idx]

        detector = mock.MagicMock()
        encoder = mock.MagicMock()

        pipeline = Recognition(
            loader,
            detector=detector,
            encoder=encoder,
            tolerance=tolerance,
            confirm_frames=1,
        )
        return pipeline

    @mock.patch("ai.recognition.face_recognition")
    def test_returns_closest_talent(self, mock_fr):
        """_match_all must return the talent with the lowest distance."""
        known = [np.random.rand(128) for _ in range(3)]
        talents = [
            {"name": "Alice"},
            {"name": "Bob"},
            {"name": "Carol"},
        ]
        # Bob (index 1) has the smallest distance
        mock_fr.face_distance.return_value = np.array([0.5, 0.2, 0.6])

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        query = [np.random.rand(128)]
        results = pipeline._match_all(query)

        assert len(results) == 1
        talent, confidence = results[0]
        assert talent["name"] == "Bob"
        assert abs(confidence - 0.8) < 1e-6

    @mock.patch("ai.recognition.face_recognition")
    def test_no_match_when_above_threshold(self, mock_fr):
        """All distances above tolerance → (None, 0.0)."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.6])

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        results = pipeline._match_all([np.random.rand(128)])

        assert results == [(None, 0.0)]

    @mock.patch("ai.recognition.face_recognition")
    def test_exact_threshold_boundary(self, mock_fr):
        """Distance exactly equal to tolerance should match."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.45])

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        results = pipeline._match_all([np.random.rand(128)])

        talent, confidence = results[0]
        assert talent["name"] == "Alice"
        assert abs(confidence - 0.55) < 1e-6

    @mock.patch("ai.recognition.face_recognition")
    def test_custom_tolerance(self, mock_fr):
        """Custom tolerance=0.3 should reject distance=0.35."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.35])

        pipeline = self._make_pipeline(known, talents, tolerance=0.3)
        results = pipeline._match_all([np.random.rand(128)])

        assert results == [(None, 0.0)]

    @mock.patch("ai.recognition.face_recognition")
    def test_custom_tolerance_accepts_close(self, mock_fr):
        """Custom tolerance=0.6 should accept distance=0.55."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.55])

        pipeline = self._make_pipeline(known, talents, tolerance=0.6)
        results = pipeline._match_all([np.random.rand(128)])

        talent, confidence = results[0]
        assert talent["name"] == "Alice"
        assert abs(confidence - 0.45) < 1e-6

    @mock.patch("ai.recognition.face_recognition")
    def test_empty_database_returns_none(self, mock_fr):
        """Empty talent database → all results are (None, 0.0)."""
        pipeline = self._make_pipeline([], [], tolerance=0.45)
        results = pipeline._match_all([np.random.rand(128)])

        assert results == [(None, 0.0)]

    @mock.patch("ai.recognition.face_recognition")
    def test_empty_encodings_returns_empty(self, mock_fr):
        """No query encodings → empty result list."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        results = pipeline._match_all([])

        assert results == []

    @mock.patch("ai.recognition.face_recognition")
    def test_multiple_queries(self, mock_fr):
        """Multiple query encodings return independent results."""
        known = [np.random.rand(128), np.random.rand(128)]
        talents = [{"name": "Alice"}, {"name": "Bob"}]

        # First query matches Alice, second matches nobody
        mock_fr.face_distance.side_effect = [
            np.array([0.1, 0.8]),
            np.array([0.6, 0.7]),
        ]

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        queries = [np.random.rand(128), np.random.rand(128)]
        results = pipeline._match_all(queries)

        assert len(results) == 2
        assert results[0][0]["name"] == "Alice"
        assert results[0][1] == pytest.approx(0.9)
        assert results[1] == (None, 0.0)

    @mock.patch("ai.recognition.face_recognition")
    def test_uses_face_distance_not_compare_faces(self, mock_fr):
        """_match_all must call face_distance, never compare_faces."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.2])

        pipeline = self._make_pipeline(known, talents)
        pipeline._match_all([np.random.rand(128)])

        mock_fr.face_distance.assert_called_once()
        mock_fr.compare_faces.assert_not_called()

    @mock.patch("ai.recognition.face_recognition")
    def test_confidence_log_on_match(self, mock_fr, caplog):
        """A successful match must emit an INFO log with confidence."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.2])

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        with caplog.at_level(logging.INFO, logger="ai.recognition"):
            pipeline._match_all([np.random.rand(128)])

        assert any("Match: Alice" in msg for msg in caplog.messages)
        assert any("confidence=0.8000" in msg for msg in caplog.messages)
        assert any("threshold=0.4500" in msg for msg in caplog.messages)

    @mock.patch("ai.recognition.face_recognition")
    def test_debug_log_on_no_match(self, mock_fr, caplog):
        """A failed match must emit a DEBUG log."""
        known = [np.random.rand(128)]
        talents = [{"name": "Alice"}]
        mock_fr.face_distance.return_value = np.array([0.6])

        pipeline = self._make_pipeline(known, talents, tolerance=0.45)
        with caplog.at_level(logging.DEBUG, logger="ai.recognition"):
            pipeline._match_all([np.random.rand(128)])

        assert any("No match" in msg for msg in caplog.messages)
        assert any("threshold=0.4500" in msg for msg in caplog.messages)

    @mock.patch("ai.recognition.face_recognition")
    def test_default_tolerance_is_045(self, mock_fr):
        """Default tolerance parameter should be 0.45."""
        from ai.recognition import Recognition

        loader = mock.MagicMock()
        loader.encodings = []
        pipeline = Recognition(
            loader,
            detector=mock.MagicMock(),
            encoder=mock.MagicMock(),
        )
        assert pipeline.tolerance == 0.45


# =====================================================================
# face_matcher.py – match() tests
# =====================================================================

class TestFaceMatcherMatch:
    """Tests for FaceMatcher.match optimized matching."""

    @mock.patch("ai.face_matcher.face_recognition")
    def test_match_uses_face_distance(self, mock_fr):
        """match() must use face_distance, not compare_faces."""
        from ai.face_matcher import FaceMatcher

        db = mock.MagicMock()
        db.encodings = [np.random.rand(128)]
        db.get_talent.return_value = {"name": "Alice"}

        mock_fr.face_distance.return_value = np.array([0.2])

        matcher = FaceMatcher(db, tolerance=0.45)
        result = matcher.match(np.random.rand(128))

        assert result["name"] == "Alice"
        mock_fr.face_distance.assert_called_once()
        mock_fr.compare_faces.assert_not_called()

    @mock.patch("ai.face_matcher.face_recognition")
    def test_match_returns_closest(self, mock_fr):
        """match() returns the talent with the lowest distance."""
        from ai.face_matcher import FaceMatcher

        db = mock.MagicMock()
        db.encodings = [np.random.rand(128) for _ in range(3)]
        db.get_talent.side_effect = lambda i: [
            {"name": "Alice"},
            {"name": "Bob"},
            {"name": "Carol"},
        ][i]

        # Carol (index 2) is closest
        mock_fr.face_distance.return_value = np.array([0.4, 0.35, 0.1])

        matcher = FaceMatcher(db, tolerance=0.45)
        result = matcher.match(np.random.rand(128))

        assert result["name"] == "Carol"

    @mock.patch("ai.face_matcher.face_recognition")
    def test_match_returns_none_above_threshold(self, mock_fr):
        """match() returns None when best distance exceeds tolerance."""
        from ai.face_matcher import FaceMatcher

        db = mock.MagicMock()
        db.encodings = [np.random.rand(128)]
        mock_fr.face_distance.return_value = np.array([0.6])

        matcher = FaceMatcher(db, tolerance=0.45)
        result = matcher.match(np.random.rand(128))

        assert result is None

    @mock.patch("ai.face_matcher.face_recognition")
    def test_match_empty_db_returns_none(self, mock_fr):
        """match() returns None for empty talent database."""
        from ai.face_matcher import FaceMatcher

        db = mock.MagicMock()
        db.encodings = []

        matcher = FaceMatcher(db, tolerance=0.45)
        result = matcher.match(np.random.rand(128))

        assert result is None

    @mock.patch("ai.face_matcher.face_recognition")
    def test_match_confidence_log(self, mock_fr, caplog):
        """match() emits INFO log with confidence on match."""
        from ai.face_matcher import FaceMatcher

        db = mock.MagicMock()
        db.encodings = [np.random.rand(128)]
        db.get_talent.return_value = {"name": "Alice"}
        mock_fr.face_distance.return_value = np.array([0.1])

        matcher = FaceMatcher(db, tolerance=0.45)
        with caplog.at_level(logging.INFO, logger="ai.face_matcher"):
            matcher.match(np.random.rand(128))

        assert any("Match: Alice" in msg for msg in caplog.messages)
        assert any("confidence=0.9000" in msg for msg in caplog.messages)

    @mock.patch("ai.face_matcher.face_recognition")
    def test_match_custom_tolerance(self, mock_fr):
        """match() respects custom tolerance parameter."""
        from ai.face_matcher import FaceMatcher

        db = mock.MagicMock()
        db.encodings = [np.random.rand(128)]
        db.get_talent.return_value = {"name": "Alice"}
        mock_fr.face_distance.return_value = np.array([0.35])

        # tolerance=0.3 → distance 0.35 exceeds threshold
        matcher = FaceMatcher(db, tolerance=0.3)
        assert matcher.match(np.random.rand(128)) is None

        # tolerance=0.4 → distance 0.35 is within threshold
        matcher2 = FaceMatcher(db, tolerance=0.4)
        result = matcher2.match(np.random.rand(128))
        assert result["name"] == "Alice"
