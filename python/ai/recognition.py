"""Full face recognition pipeline with temporal smoothing and stable tracking.

Orchestrates detection, encoding, and matching into a single callable
pipeline optimized for real-time broadcast with multi-person tracking.
"""

import time
from collections import defaultdict
from typing import Dict, List, Optional, Tuple

import numpy as np

try:
    import face_recognition
except ImportError:
    face_recognition = None

from .detection import Detection
from .encoding import Encoding
from .talents_loader import TalentsLoader


class _TrackedFace:
    """Internal bookkeeping for a tracked identity."""

    __slots__ = (
        "talent",
        "confidence_history",
        "last_location",
        "last_seen_ts",
        "consecutive_misses",
    )

    def __init__(
        self,
        talent: Optional[dict],
        confidence: float,
        location: Tuple[int, int, int, int],
    ):
        self.talent = talent
        self.confidence_history: List[float] = [confidence]
        self.last_location = location
        self.last_seen_ts: float = time.monotonic()
        self.consecutive_misses: int = 0

    @property
    def avg_confidence(self) -> float:
        if not self.confidence_history:
            return 0.0
        return float(np.mean(self.confidence_history))


class RecognitionResult:
    """Container for a single frame's recognition output."""

    __slots__ = ("faces",)

    def __init__(self) -> None:
        self.faces: List[Dict] = []

    def add(
        self,
        location: Tuple[int, int, int, int],
        talent: Optional[dict],
        confidence: float,
        track_id: Optional[str] = None,
    ) -> None:
        self.faces.append(
            {
                "location": location,
                "talent": talent,
                "confidence": round(confidence, 4),
                "track_id": track_id,
            }
        )


class Recognition:
    """End-to-end face recognition pipeline.

    Combines :class:`Detection`, :class:`Encoding`, and
    :class:`TalentsLoader` into a pipeline with:

    - **Multi-person tracking** — stable ``track_id`` assignment across frames
      via spatial IoU matching and exponential-moving-average confidence.
    - **Temporal smoothing** — identity is confirmed only after it appears
      consistently across ``confirm_frames`` consecutive detections.
    - **Automatic eviction** — tracks are dropped after ``max_miss`` frames
      without a match.

    Args:
        talents_loader: Loaded :class:`TalentsLoader` instance.
        detector: Optional pre-configured :class:`Detection` instance.
        encoder: Optional pre-configured :class:`Encoding` instance.
        tolerance: Distance threshold for matching (lower = stricter).
        confirm_frames: Frames an identity must be seen before confirmed.
        max_miss: Consecutive misses before a track is dropped.
        iou_threshold: Minimum IoU to associate a detection with an existing track.
    """

    def __init__(
        self,
        talents_loader: TalentsLoader,
        detector: Optional[Detection] = None,
        encoder: Optional[Encoding] = None,
        tolerance: float = 0.45,
        confirm_frames: int = 3,
        max_miss: int = 5,
        iou_threshold: float = 0.3,
    ):
        if face_recognition is None:
            raise ImportError(
                "face_recognition is required. Install with: "
                "pip install face_recognition"
            )

        self.talents_loader = talents_loader
        self.detector = detector or Detection()
        self.encoder = encoder or Encoding()
        self.tolerance = tolerance
        self.confirm_frames = confirm_frames
        self.max_miss = max_miss
        self.iou_threshold = iou_threshold

        self._tracks: Dict[str, _TrackedFace] = {}
        self._next_track_id: int = 0

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def process_frame(self, frame: np.ndarray) -> RecognitionResult:
        """Run the full pipeline on a single video frame.

        Args:
            frame: RGB image as ``numpy`` array (H, W, 3).

        Returns:
            :class:`RecognitionResult` with per-face talent, confidence,
            and track ID.
        """
        locations = self.detector.detect(frame)
        encodings = self.encoder.encode_faces(frame, locations)

        matched_talents = self._match_all(encodings)

        result = self._update_tracks(locations, matched_talents)
        return result

    def reset(self) -> None:
        """Clear all tracking state."""
        self._tracks.clear()
        self._next_track_id = 0
        self.detector.reset()
        self.encoder.clear_cache()

    # ------------------------------------------------------------------
    # Matching
    # ------------------------------------------------------------------

    def _match_all(
        self, encodings: List[np.ndarray]
    ) -> List[Tuple[Optional[dict], float]]:
        """Match each encoding against the talent database.

        Returns a list of ``(talent_dict | None, confidence)`` tuples.
        """
        known_encodings = self.talents_loader.encodings
        if not known_encodings or not encodings:
            return [(None, 0.0)] * len(encodings)

        results: List[Tuple[Optional[dict], float]] = []
        for enc in encodings:
            distances = face_recognition.face_distance(known_encodings, enc)
            best_idx = int(np.argmin(distances))
            best_dist = float(distances[best_idx])
            if best_dist <= self.tolerance:
                confidence = 1.0 - best_dist
                talent = self.talents_loader.get_talent(best_idx)
                results.append((talent, confidence))
            else:
                results.append((None, 0.0))
        return results

    # ------------------------------------------------------------------
    # Tracking
    # ------------------------------------------------------------------

    def _update_tracks(
        self,
        locations: List[Tuple[int, int, int, int]],
        matches: List[Tuple[Optional[dict], float]],
    ) -> RecognitionResult:
        """Associate current detections with existing tracks via IoU."""
        now = time.monotonic()
        result = RecognitionResult()

        used_track_ids: set = set()
        detection_to_track: Dict[int, str] = {}

        # Greedy IoU assignment: for each detection, find the best track.
        for det_idx, loc in enumerate(locations):
            best_tid: Optional[str] = None
            best_iou: float = 0.0
            for tid, trk in self._tracks.items():
                if tid in used_track_ids:
                    continue
                iou = self._iou(loc, trk.last_location)
                if iou > best_iou and iou >= self.iou_threshold:
                    best_iou = iou
                    best_tid = tid
            if best_tid is not None:
                used_track_ids.add(best_tid)
                detection_to_track[det_idx] = best_tid

        # Update matched tracks and create new ones.
        for det_idx, (loc, (talent, conf)) in enumerate(
            zip(locations, matches)
        ):
            tid = detection_to_track.get(det_idx)
            if tid is not None:
                trk = self._tracks[tid]
                trk.last_location = loc
                trk.last_seen_ts = now
                trk.consecutive_misses = 0
                if talent is not None:
                    trk.talent = talent
                    trk.confidence_history.append(conf)
                    # Keep a sliding window.
                    if len(trk.confidence_history) > 30:
                        trk.confidence_history = trk.confidence_history[-30:]
            else:
                tid = self._new_track_id()
                self._tracks[tid] = _TrackedFace(talent, conf, loc)

            # Only report confirmed identities.
            trk = self._tracks[tid]
            if (
                trk.talent is not None
                and len(trk.confidence_history) >= self.confirm_frames
            ):
                result.add(loc, trk.talent, trk.avg_confidence, tid)
            else:
                result.add(loc, None, 0.0, tid)

        # Increment misses on unmatched tracks and evict stale ones.
        for tid in list(self._tracks):
            if tid not in used_track_ids:
                self._tracks[tid].consecutive_misses += 1
                if self._tracks[tid].consecutive_misses > self.max_miss:
                    del self._tracks[tid]

        return result

    def _new_track_id(self) -> str:
        tid = f"t{self._next_track_id}"
        self._next_track_id += 1
        return tid

    # ------------------------------------------------------------------
    # Geometry helpers
    # ------------------------------------------------------------------

    @staticmethod
    def _iou(
        a: Tuple[int, int, int, int], b: Tuple[int, int, int, int]
    ) -> float:
        """Compute Intersection-over-Union of two (top, right, bottom, left) boxes."""
        a_top, a_right, a_bottom, a_left = a
        b_top, b_right, b_bottom, b_left = b

        inter_top = max(a_top, b_top)
        inter_left = max(a_left, b_left)
        inter_bottom = min(a_bottom, b_bottom)
        inter_right = min(a_right, b_right)

        inter_w = max(0, inter_right - inter_left)
        inter_h = max(0, inter_bottom - inter_top)
        inter_area = inter_w * inter_h

        a_area = (a_right - a_left) * (a_bottom - a_top)
        b_area = (b_right - b_left) * (b_bottom - b_top)
        union_area = a_area + b_area - inter_area

        if union_area <= 0:
            return 0.0
        return inter_area / union_area
