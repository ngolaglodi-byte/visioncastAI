"""VisionCast-AI face detection and recognition sub-module."""

from .face_detector import FaceDetector
from .face_matcher import FaceMatcher
from .talent_db import TalentDB

__all__ = ["FaceDetector", "FaceMatcher", "TalentDB"]
