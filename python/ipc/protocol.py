"""Message schema definitions for the Python <-> C++ IPC protocol.

All messages are serialized as JSON over ZeroMQ PUB/SUB sockets.
See docs/ARCHITECTURE.md Section 8 for the full protocol specification.
"""

import json
import time
from dataclasses import dataclass, field, asdict
from typing import Optional


@dataclass
class FaceLocation:
    """Bounding box of a detected face in the frame."""
    top: int
    right: int
    bottom: int
    left: int


@dataclass
class TalentInfo:
    """Metadata about a recognized talent."""
    id: str
    name: str
    role: str
    overlay_template: str = ""


@dataclass
class RecognizedFace:
    """A single recognized face with location, talent info, and confidence."""
    location: FaceLocation
    talent: Optional[TalentInfo] = None
    confidence: float = 0.0


@dataclass
class RecognitionResult:
    """Complete recognition result for a single frame."""
    timestamp_ms: int = 0
    frame_id: int = 0
    faces: list = field(default_factory=list)

    def to_json(self) -> str:
        """Serialize to JSON string for ZeroMQ transmission."""
        data = {
            "type": "recognition_result",
            "timestamp_ms": self.timestamp_ms,
            "frame_id": self.frame_id,
            "faces": [],
        }
        for face in self.faces:
            face_data = {
                "location": asdict(face.location),
                "confidence": face.confidence,
                "talent": asdict(face.talent) if face.talent else None,
            }
            data["faces"].append(face_data)
        return json.dumps(data)

    @classmethod
    def from_json(cls, json_str: str) -> "RecognitionResult":
        """Deserialize from JSON string."""
        data = json.loads(json_str)
        faces = []
        for face_data in data.get("faces", []):
            loc = FaceLocation(**face_data["location"])
            talent = None
            if face_data.get("talent"):
                talent = TalentInfo(**face_data["talent"])
            faces.append(
                RecognizedFace(
                    location=loc,
                    talent=talent,
                    confidence=face_data.get("confidence", 0.0),
                )
            )
        return cls(
            timestamp_ms=data.get("timestamp_ms", 0),
            frame_id=data.get("frame_id", 0),
            faces=faces,
        )


@dataclass
class Heartbeat:
    """Health check message for module monitoring."""
    module: str
    status: str = "running"
    fps: float = 0.0
    timestamp_ms: int = 0

    def to_json(self) -> str:
        """Serialize to JSON string."""
        return json.dumps({
            "type": "heartbeat",
            "module": self.module,
            "timestamp_ms": self.timestamp_ms or int(time.time() * 1000),
            "status": self.status,
            "fps": self.fps,
        })
