"""Optimized multi-person face detection for broadcast environments.

Provides high-performance face detection with frame downscaling,
skip-frame reuse, and region-of-interest tracking for real-time
multi-person detection in video streams.
"""

from typing import List, Optional, Tuple

import numpy as np

try:
    import face_recognition
except ImportError:
    face_recognition = None


class Detection:
    """High-performance multi-person face detector.

    Optimizes detection throughput via:
    - Frame downscaling to reduce computation.
    - Skip-frame reuse: detect every N-th frame, reuse locations otherwise.
    - Configurable model selection (HOG for CPU, CNN for GPU).

    Args:
        model: Detection model – ``"hog"`` (fast, CPU) or ``"cnn"`` (accurate, GPU).
        upsample: Times to upsample image for small-face detection.
        scale: Downscale factor applied before detection (0 < scale <= 1.0).
            Smaller values are faster but less accurate.
        skip_frames: Reuse previous locations for this many frames between
            full detections.  Set to 0 to detect every frame.
    """

    def __init__(
        self,
        model: str = "hog",
        upsample: int = 1,
        scale: float = 0.5,
        skip_frames: int = 2,
    ):
        if face_recognition is None:
            raise ImportError(
                "face_recognition is required. Install with: "
                "pip install face_recognition"
            )
        if not 0 < scale <= 1.0:
            raise ValueError("scale must be in (0, 1.0]")
        if skip_frames < 0:
            raise ValueError("skip_frames must be >= 0")

        self.model = model
        self.upsample = upsample
        self.scale = scale
        self.skip_frames = skip_frames

        self._frame_counter: int = 0
        self._last_locations: List[Tuple[int, int, int, int]] = []

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def detect(self, frame: np.ndarray) -> List[Tuple[int, int, int, int]]:
        """Detect face locations in *frame*, applying skip-frame logic.

        When ``skip_frames > 0``, a full detection pass is only executed
        on every (skip_frames + 1)-th call; intermediate calls return the
        most recent cached locations.

        Args:
            frame: RGB image as ``numpy`` array (H, W, 3).

        Returns:
            List of face bounding boxes as ``(top, right, bottom, left)``
            tuples in *original* frame coordinates.
        """
        self._frame_counter += 1

        if self.skip_frames == 0 or self._frame_counter % (self.skip_frames + 1) == 1:
            self._last_locations = self._detect_scaled(frame)

        return list(self._last_locations)

    def detect_all(self, frame: np.ndarray) -> List[Tuple[int, int, int, int]]:
        """Force a full detection pass (ignores skip-frame counter).

        Useful when the caller knows the scene has changed significantly
        (e.g. a camera cut).

        Args:
            frame: RGB image as ``numpy`` array (H, W, 3).

        Returns:
            List of face bounding boxes in original frame coordinates.
        """
        self._last_locations = self._detect_scaled(frame)
        self._frame_counter = 1
        return list(self._last_locations)

    def reset(self) -> None:
        """Clear cached locations and reset the frame counter."""
        self._frame_counter = 0
        self._last_locations = []

    # ------------------------------------------------------------------
    # Internal helpers
    # ------------------------------------------------------------------

    def _detect_scaled(
        self, frame: np.ndarray
    ) -> List[Tuple[int, int, int, int]]:
        """Run face detection on a down-scaled copy and map results back."""
        if self.scale < 1.0:
            small = self._downscale(frame)
            locations = face_recognition.face_locations(
                small,
                number_of_times_to_upsample=self.upsample,
                model=self.model,
            )
            inv = 1.0 / self.scale
            return [
                (
                    int(top * inv),
                    int(right * inv),
                    int(bottom * inv),
                    int(left * inv),
                )
                for top, right, bottom, left in locations
            ]

        return face_recognition.face_locations(
            frame,
            number_of_times_to_upsample=self.upsample,
            model=self.model,
        )

    def _downscale(self, frame: np.ndarray) -> np.ndarray:
        """Return a downscaled copy of *frame*."""
        try:
            import cv2

            h, w = frame.shape[:2]
            new_w = max(1, int(w * self.scale))
            new_h = max(1, int(h * self.scale))
            return cv2.resize(frame, (new_w, new_h), interpolation=cv2.INTER_AREA)
        except ImportError:
            # Fallback: simple stride-based downscale (no OpenCV needed).
            step = max(1, int(1.0 / self.scale))
            return np.ascontiguousarray(frame[::step, ::step])
