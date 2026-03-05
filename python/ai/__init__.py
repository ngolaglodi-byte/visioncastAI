"""VisionCast-AI face detection and recognition sub-module."""

from .face_detector import FaceDetector
from .face_matcher import FaceMatcher
from .talent_db import TalentDB

from .detection import Detection
from .encoding import Encoding
from .recognition import Recognition
from .talents_loader import TalentsLoader

__all__ = [
    "FaceDetector",
    "FaceMatcher",
    "TalentDB",
    "Detection",
    "Encoding",
    "Recognition",
    "TalentsLoader",
]
