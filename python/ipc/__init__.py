"""VisionCast-AI inter-process communication sub-module."""

from .protocol import (
    FaceLocation,
    TalentInfo,
    RecognizedFace,
    RecognitionResult,
)
from .metadata_sender import MetadataSender

__all__ = [
    "FaceLocation",
    "TalentInfo",
    "RecognizedFace",
    "RecognitionResult",
    "MetadataSender",
]
