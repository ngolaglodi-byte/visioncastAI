"""ZeroMQ publisher for sending talent overlay metadata to the C++ engine.

Publishes rich talent identification data (talent_id, name, role,
organization, overlay, theme_color, filters, animations, confidence)
over ZeroMQ PUB/SUB for real-time overlay rendering.
"""

try:
    import zmq
except ImportError:
    zmq = None

from .protocol import TalentOverlayMessage


class ZmqSender:
    """Publishes talent overlay metadata to the C++ engine via ZeroMQ.

    Args:
        endpoint: ZeroMQ endpoint (default: tcp://127.0.0.1:5556).
    """

    TOPIC_TALENT_OVERLAY = b"talent.overlay"

    def __init__(self, endpoint: str = "tcp://127.0.0.1:5556"):
        if zmq is None:
            raise ImportError(
                "pyzmq is required. Install with: pip install pyzmq"
            )
        self.endpoint = endpoint
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PUB)
        self.socket.bind(endpoint)

    def send(self, message: TalentOverlayMessage) -> None:
        """Send a talent overlay message to the C++ engine.

        Args:
            message: TalentOverlayMessage with talent metadata.
        """
        payload = message.to_json().encode("utf-8")
        self.socket.send_multipart([self.TOPIC_TALENT_OVERLAY, payload])

    def send_talent(
        self,
        talent_id: str,
        name: str,
        role: str,
        organization: str = "",
        overlay: str = "",
        theme_color: str = "#FFFFFF",
        filters: dict = None,
        animations: dict = None,
        confidence: float = 0.0,
    ) -> None:
        """Convenience method to send talent data from individual fields.

        Args:
            talent_id: Unique identifier for the talent.
            name: Display name of the talent.
            role: Role or title of the talent.
            organization: Organization the talent belongs to.
            overlay: Path to the overlay template.
            theme_color: Hex color for the overlay theme.
            filters: Visual filter settings (brightness, contrast, etc.).
            animations: Animation settings (entry, exit, duration, etc.).
            confidence: Face recognition confidence score (0.0 to 1.0).
        """
        message = TalentOverlayMessage(
            talent_id=talent_id,
            name=name,
            role=role,
            organization=organization,
            overlay=overlay,
            theme_color=theme_color,
            filters=filters if filters is not None else {},
            animations=animations if animations is not None else {},
            confidence=confidence,
        )
        self.send(message)

    def close(self) -> None:
        """Close the ZeroMQ socket and context."""
        self.socket.close()
        self.context.term()
