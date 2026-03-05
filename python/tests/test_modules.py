"""Unit tests for the optimized face recognition modules.

These tests validate core logic (caching, tracking, IoU, skip-frames, etc.)
using mocked face_recognition calls so they run fast and without GPU/dlib.
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


# =====================================================================
# detection.py tests
# =====================================================================

class TestDetection:
    """Tests for the Detection module."""

    def test_init_defaults(self):
        from ai.detection import Detection

        det = Detection()
        assert det.model == "hog"
        assert det.scale == 0.5
        assert det.skip_frames == 2

    def test_invalid_scale_raises(self):
        from ai.detection import Detection

        with pytest.raises(ValueError, match="scale"):
            Detection(scale=0.0)
        with pytest.raises(ValueError, match="scale"):
            Detection(scale=1.5)

    def test_invalid_skip_frames_raises(self):
        from ai.detection import Detection

        with pytest.raises(ValueError, match="skip_frames"):
            Detection(skip_frames=-1)

    @mock.patch("ai.detection.face_recognition")
    def test_skip_frame_reuse(self, mock_fr):
        from ai.detection import Detection

        fake_locations = [(10, 110, 60, 10)]
        mock_fr.face_locations.return_value = fake_locations

        det = Detection(scale=1.0, skip_frames=2)
        frame = np.zeros((100, 100, 3), dtype=np.uint8)

        # Frame 1: full detection
        locs = det.detect(frame)
        assert locs == fake_locations
        assert mock_fr.face_locations.call_count == 1

        # Frames 2-3: skip, return cached
        locs2 = det.detect(frame)
        locs3 = det.detect(frame)
        assert locs2 == fake_locations
        assert locs3 == fake_locations
        assert mock_fr.face_locations.call_count == 1  # No new calls

        # Frame 4: full detection again
        det.detect(frame)
        assert mock_fr.face_locations.call_count == 2

    @mock.patch("ai.detection.face_recognition")
    def test_detect_all_forces_full_pass(self, mock_fr):
        from ai.detection import Detection

        mock_fr.face_locations.return_value = [(5, 50, 55, 5)]
        det = Detection(scale=1.0, skip_frames=5)
        frame = np.zeros((100, 100, 3), dtype=np.uint8)

        locs = det.detect_all(frame)
        assert locs == [(5, 50, 55, 5)]
        assert mock_fr.face_locations.call_count == 1

    @mock.patch("ai.detection.face_recognition")
    def test_scale_rescales_locations(self, mock_fr):
        from ai.detection import Detection

        # At 0.5 scale, detected at (5,25,30,5) → original (10,50,60,10)
        mock_fr.face_locations.return_value = [(5, 25, 30, 5)]
        det = Detection(scale=0.5, skip_frames=0)
        frame = np.zeros((100, 100, 3), dtype=np.uint8)

        locs = det.detect(frame)
        assert locs == [(10, 50, 60, 10)]

    def test_reset_clears_state(self):
        from ai.detection import Detection

        det = Detection()
        det._frame_counter = 10
        det._last_locations = [(1, 2, 3, 4)]
        det.reset()
        assert det._frame_counter == 0
        assert det._last_locations == []


# =====================================================================
# encoding.py tests
# =====================================================================

class TestEncoding:
    """Tests for the Encoding module."""

    def test_init_defaults(self):
        from ai.encoding import Encoding

        enc = Encoding()
        assert enc.num_jitters == 1
        assert enc.cache_size == 128

    def test_invalid_jitters_raises(self):
        from ai.encoding import Encoding

        with pytest.raises(ValueError, match="num_jitters"):
            Encoding(num_jitters=0)

    @mock.patch("ai.encoding.face_recognition")
    def test_encode_faces_returns_encodings(self, mock_fr):
        from ai.encoding import Encoding

        fake_enc = np.random.rand(128)
        mock_fr.face_encodings.return_value = [fake_enc]

        encoder = Encoding(cache_size=0)
        frame = np.zeros((100, 100, 3), dtype=np.uint8)
        result = encoder.encode_faces(frame, [(10, 50, 60, 10)])

        assert len(result) == 1
        np.testing.assert_array_equal(result[0], fake_enc)

    @mock.patch("ai.encoding.face_recognition")
    def test_cache_hit_avoids_recompute(self, mock_fr):
        from ai.encoding import Encoding

        fake_enc = np.random.rand(128)
        mock_fr.face_encodings.return_value = [fake_enc]

        encoder = Encoding(cache_size=10)
        frame = np.zeros((100, 100, 3), dtype=np.uint8)
        loc = (10, 50, 60, 10)

        # First call: cache miss → compute
        result1 = encoder.encode_faces(frame, [loc])
        assert mock_fr.face_encodings.call_count == 1

        # Second call: cache hit → no compute
        result2 = encoder.encode_faces(frame, [loc])
        assert mock_fr.face_encodings.call_count == 1
        np.testing.assert_array_equal(result1[0], result2[0])

    def test_cache_stats(self):
        from ai.encoding import Encoding

        encoder = Encoding(cache_size=64)
        stats = encoder.cache_stats
        assert stats == {"size": 0, "capacity": 64}

    @mock.patch("ai.encoding.face_recognition")
    def test_empty_locations_returns_empty(self, mock_fr):
        from ai.encoding import Encoding

        encoder = Encoding()
        result = encoder.encode_faces(np.zeros((10, 10, 3), dtype=np.uint8), [])
        assert result == []

    @mock.patch("ai.encoding.face_recognition")
    def test_cache_eviction(self, mock_fr):
        from ai.encoding import Encoding

        encoder = Encoding(cache_size=2)
        frame = np.zeros((100, 100, 3), dtype=np.uint8)

        for i in range(5):
            mock_fr.face_encodings.return_value = [np.random.rand(128)]
            encoder.encode_faces(frame, [(i * 10, 50 + i, 60 + i, i)])

        assert encoder.cache_stats["size"] == 2


# =====================================================================
# talents_loader.py tests
# =====================================================================

class TestTalentsLoader:
    """Tests for the TalentsLoader module."""

    @mock.patch("ai.talents_loader.face_recognition")
    def test_load_from_json(self, mock_fr):
        from ai.talents_loader import TalentsLoader

        fake_enc = np.random.rand(128)
        mock_fr.load_image_file.return_value = np.zeros((100, 100, 3))
        mock_fr.face_encodings.return_value = [fake_enc]

        with tempfile.TemporaryDirectory() as tmpdir:
            # Create a minimal talents.json and a fake image
            talents = {
                "talents": [
                    {
                        "id": "test",
                        "name": "Test Person",
                        "role": "Host",
                        "image": "img/test.jpg",
                    }
                ]
            }
            json_path = os.path.join(tmpdir, "talents.json")
            with open(json_path, "w") as f:
                json.dump(talents, f)

            img_dir = os.path.join(tmpdir, "img")
            os.makedirs(img_dir)
            # Create a dummy image file
            with open(os.path.join(img_dir, "test.jpg"), "wb") as f:
                f.write(b"\x00" * 100)

            loader = TalentsLoader(json_path, project_root=tmpdir)
            count = loader.load()

            assert count == 1
            assert loader.count() == 1
            assert loader.get_talent(0)["name"] == "Test Person"

    @mock.patch("ai.talents_loader.face_recognition")
    def test_find_by_name(self, mock_fr):
        from ai.talents_loader import TalentsLoader

        fake_enc = np.random.rand(128)
        mock_fr.load_image_file.return_value = np.zeros((100, 100, 3))
        mock_fr.face_encodings.return_value = [fake_enc]

        with tempfile.TemporaryDirectory() as tmpdir:
            talents = {
                "talents": [
                    {
                        "id": "alice",
                        "name": "Alice",
                        "role": "Host",
                        "image": "img/alice.jpg",
                    }
                ]
            }
            json_path = os.path.join(tmpdir, "talents.json")
            with open(json_path, "w") as f:
                json.dump(talents, f)

            img_dir = os.path.join(tmpdir, "img")
            os.makedirs(img_dir)
            with open(os.path.join(img_dir, "alice.jpg"), "wb") as f:
                f.write(b"\x00" * 100)

            loader = TalentsLoader(json_path, project_root=tmpdir)
            loader.load()

            result = loader.find_by_name("alice")
            assert result is not None
            assert result[0]["name"] == "Alice"

            assert loader.find_by_name("nobody") is None

    @mock.patch("ai.talents_loader.face_recognition")
    def test_reload_if_changed(self, mock_fr):
        from ai.talents_loader import TalentsLoader

        fake_enc = np.random.rand(128)
        mock_fr.load_image_file.return_value = np.zeros((100, 100, 3))
        mock_fr.face_encodings.return_value = [fake_enc]

        with tempfile.TemporaryDirectory() as tmpdir:
            talents = {"talents": []}
            json_path = os.path.join(tmpdir, "talents.json")
            with open(json_path, "w") as f:
                json.dump(talents, f)

            loader = TalentsLoader(json_path, project_root=tmpdir)
            loader.load()

            # No change → no reload
            assert loader.reload_if_changed() is False

            # Change file → should reload
            with open(json_path, "w") as f:
                json.dump({"talents": []}, f)
                f.write(" ")  # Ensure different content

            assert loader.reload_if_changed() is True


# =====================================================================
# recognition.py tests
# =====================================================================

class TestRecognition:
    """Tests for the Recognition pipeline."""

    def test_iou_calculation(self):
        from ai.recognition import Recognition

        # Perfect overlap
        assert Recognition._iou((0, 10, 10, 0), (0, 10, 10, 0)) == 1.0

        # No overlap
        assert Recognition._iou((0, 10, 10, 0), (20, 30, 30, 20)) == 0.0

        # Partial overlap: 25 / (100 + 100 - 25) = 25/175
        iou = Recognition._iou((0, 10, 10, 0), (5, 15, 15, 5))
        assert abs(iou - 25 / 175) < 1e-6

    def test_recognition_result_container(self):
        from ai.recognition import RecognitionResult

        result = RecognitionResult()
        result.add((10, 50, 60, 10), {"name": "Alice"}, 0.95, "t0")
        result.add((70, 130, 120, 70), None, 0.0, "t1")

        assert len(result.faces) == 2
        assert result.faces[0]["talent"]["name"] == "Alice"
        assert result.faces[0]["track_id"] == "t0"
        assert result.faces[1]["talent"] is None

    @mock.patch("ai.detection.face_recognition")
    @mock.patch("ai.encoding.face_recognition")
    @mock.patch("ai.recognition.face_recognition")
    @mock.patch("ai.talents_loader.face_recognition")
    def test_process_frame_end_to_end(
        self, mock_fr_tl, mock_fr_rec, mock_fr_enc, mock_fr_det
    ):
        from ai.detection import Detection
        from ai.encoding import Encoding
        from ai.recognition import Recognition
        from ai.talents_loader import TalentsLoader

        # Setup: one known talent with a known encoding
        known_enc = np.random.rand(128)
        detected_enc = known_enc + np.random.rand(128) * 0.01  # very close

        mock_fr_det.face_locations.return_value = [(10, 60, 60, 10)]
        mock_fr_enc.face_encodings.return_value = [detected_enc]
        mock_fr_rec.face_distance.return_value = np.array([0.1])

        with tempfile.TemporaryDirectory() as tmpdir:
            talents_data = {
                "talents": [
                    {"id": "t1", "name": "Bob", "role": "Host", "image": "img/b.jpg"}
                ]
            }
            json_path = os.path.join(tmpdir, "talents.json")
            with open(json_path, "w") as f:
                json.dump(talents_data, f)

            os.makedirs(os.path.join(tmpdir, "img"))
            with open(os.path.join(tmpdir, "img", "b.jpg"), "wb") as f:
                f.write(b"\x00" * 10)

            mock_fr_tl.load_image_file.return_value = np.zeros((100, 100, 3))
            mock_fr_tl.face_encodings.return_value = [known_enc]

            loader = TalentsLoader(json_path, project_root=tmpdir)
            loader.load()

            detector = Detection(scale=1.0, skip_frames=0)
            encoder = Encoding(cache_size=0)
            pipeline = Recognition(
                loader, detector=detector, encoder=encoder,
                confirm_frames=1,
            )

            frame = np.zeros((100, 100, 3), dtype=np.uint8)
            result = pipeline.process_frame(frame)

            assert len(result.faces) == 1
            assert result.faces[0]["talent"]["name"] == "Bob"
            assert result.faces[0]["track_id"] is not None
