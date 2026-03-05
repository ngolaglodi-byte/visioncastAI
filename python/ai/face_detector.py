"""Face detection engine using the face_recognition library."""

import numpy as np

try:
    import face_recognition
except ImportError:
    face_recognition = None


class FaceDetector:
    """Detects face bounding boxes and computes encodings in video frames.

    Args:
        model: Detection model - "hog" (CPU, fast) or "cnn" (GPU, accurate).
        upsample: Number of times to upsample the image for detection.
    """

    def __init__(self, model: str = "hog", upsample: int = 1):
        if face_recognition is None:
            raise ImportError(
                "face_recognition is required. Install with: "
                "pip install face_recognition"
            )
        self.model = model
        self.upsample = upsample

    def detect(self, frame: np.ndarray) -> list:
        """Detect face locations in a frame.

        Args:
            frame: RGB image as numpy array (H, W, 3).

        Returns:
            List of face locations as (top, right, bottom, left) tuples.
        """
        return face_recognition.face_locations(
            frame, number_of_times_to_upsample=self.upsample, model=self.model
        )

    def encode(self, frame: np.ndarray, locations: list) -> list:
        """Compute 128-dimensional face encodings for detected faces.

        Args:
            frame: RGB image as numpy array (H, W, 3).
            locations: List of face locations from detect().

        Returns:
            List of 128-d numpy arrays (one per face).
        """
        return face_recognition.face_encodings(frame, locations)
