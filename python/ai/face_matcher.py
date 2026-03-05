"""Face matching against the known talent database."""

from typing import Optional

import numpy as np

try:
    import face_recognition
except ImportError:
    face_recognition = None

from .talent_db import TalentDB


class FaceMatcher:
    """Matches face encodings against a known talent database.

    Args:
        talent_db: Loaded TalentDB instance.
        tolerance: Distance threshold for matching (lower = stricter).
    """

    def __init__(self, talent_db: TalentDB, tolerance: float = 0.45):
        if face_recognition is None:
            raise ImportError(
                "face_recognition is required. Install with: "
                "pip install face_recognition"
            )
        self.talent_db = talent_db
        self.tolerance = tolerance

    def match(self, encoding: np.ndarray) -> Optional[dict]:
        """Match a face encoding against the talent database.

        Args:
            encoding: 128-d face encoding numpy array.

        Returns:
            Talent metadata dict if matched, None otherwise.
        """
        if not self.talent_db.encodings:
            return None

        matches = face_recognition.compare_faces(
            self.talent_db.encodings, encoding, tolerance=self.tolerance
        )
        if True in matches:
            idx = matches.index(True)
            return self.talent_db.get_talent(idx)
        return None

    def match_with_confidence(self, encoding: np.ndarray) -> tuple:
        """Match with confidence score.

        Args:
            encoding: 128-d face encoding numpy array.

        Returns:
            Tuple of (talent_dict or None, confidence float).
        """
        if not self.talent_db.encodings:
            return None, 0.0

        distances = face_recognition.face_distance(
            self.talent_db.encodings, encoding
        )
        best_idx = int(np.argmin(distances))
        best_distance = distances[best_idx]

        if best_distance <= self.tolerance:
            confidence = 1.0 - best_distance
            return self.talent_db.get_talent(best_idx), confidence
        return None, 0.0
