"""Message schema definitions for the Python <-> C++ IPC protocol.

All messages are serialized as JSON over ZeroMQ PUB/SUB sockets.
See docs/ARCHITECTURE.md Section 8 for the full protocol specification.
"""

import json
import time
from dataclasses import dataclass, field, asdict
from typing import Any, Dict, Optional


@dataclass
class TalentOverlayMessage:
    """Rich talent metadata sent to the engine for overlay rendering.

    Contains visual and identification data for a recognized talent,
    including overlay configuration, theme, filters, and animations.
    """
    talent_id: str
    name: str
    role: str
    organization: str = ""
    overlay: str = ""
    theme_color: str = "#FFFFFF"
    filters: Dict[str, Any] = field(default_factory=dict)
    animations: Dict[str, Any] = field(default_factory=dict)
    confidence: float = 0.0
    display_duration_ms: float = 5000.0

    def to_json(self) -> str:
        """Serialize to JSON string for ZeroMQ transmission."""
        return json.dumps({
            "type": "talent_overlay",
            "talent_id": self.talent_id,
            "name": self.name,
            "role": self.role,
            "organization": self.organization,
            "overlay": self.overlay,
            "theme_color": self.theme_color,
            "filters": self.filters,
            "animations": self.animations,
            "confidence": self.confidence,
            "display_duration_ms": self.display_duration_ms,
        })

    @classmethod
    def from_json(cls, json_str: str) -> "TalentOverlayMessage":
        """Deserialize from JSON string."""
        data = json.loads(json_str)
        return cls(
            talent_id=data["talent_id"],
            name=data["name"],
            role=data["role"],
            organization=data.get("organization", ""),
            overlay=data.get("overlay", ""),
            theme_color=data.get("theme_color", "#FFFFFF"),
            filters=data.get("filters", {}),
            animations=data.get("animations", {}),
            confidence=data.get("confidence", 0.0),
            display_duration_ms=data.get("display_duration_ms", 5000.0),
        )


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


@dataclass
class LogMessage:
    """Log entry sent from a module to the monitoring panel.

    Supports AI, Engine, and ZeroMQ log sources for unified
    display in the monitoring panel.
    """
    source: str
    level: str = "INFO"
    message: str = ""
    timestamp_ms: int = 0

    VALID_SOURCES = ("ai", "engine", "zmq")
    VALID_LEVELS = ("DEBUG", "INFO", "WARNING", "ERROR")

    def __post_init__(self):
        if self.source not in self.VALID_SOURCES:
            raise ValueError(
                f"Invalid source {self.source!r}, "
                f"must be one of {self.VALID_SOURCES}"
            )
        if self.level not in self.VALID_LEVELS:
            raise ValueError(
                f"Invalid level {self.level!r}, "
                f"must be one of {self.VALID_LEVELS}"
            )

    def to_json(self) -> str:
        """Serialize to JSON string for ZeroMQ transmission."""
        return json.dumps({
            "type": "log",
            "source": self.source,
            "level": self.level,
            "message": self.message,
            "timestamp_ms": self.timestamp_ms or int(time.time() * 1000),
        })

    @classmethod
    def from_json(cls, json_str: str) -> "LogMessage":
        """Deserialize from JSON string."""
        data = json.loads(json_str)
        return cls(
            source=data["source"],
            level=data.get("level", "INFO"),
            message=data.get("message", ""),
            timestamp_ms=data.get("timestamp_ms", 0),
        )
