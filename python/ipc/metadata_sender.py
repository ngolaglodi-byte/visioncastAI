"""ZeroMQ-based metadata publisher for sending AI results to the C++ engine.

Sends face recognition metadata and heartbeat messages over ZeroMQ PUB/SUB.
The C++ engine subscribes to these messages for overlay rendering.

Endpoint resolution order (highest to lowest priority):
  1. ``endpoint`` constructor argument (explicit override).
  2. ``ZMQ_PUB_ENDPOINT`` environment variable.
  3. ``ai.zmq_pub_endpoint`` key in ``config/system.json``.
  4. Built-in default ``tcp://127.0.0.1:5557``.
"""

import time

try:
    import zmq
except ImportError:
    zmq = None

from .protocol import RecognitionResult, Heartbeat
from ._config import resolve_zmq_pub_endpoint as _resolve_endpoint


class MetadataSender:
    """Publishes face recognition metadata to the C++ engine via ZeroMQ.

    Args:
        endpoint: ZeroMQ PUB endpoint to bind to.  When *None* (the default),
            the endpoint is resolved from the environment variable
            ``ZMQ_PUB_ENDPOINT``, then ``config/system.json``
            (``ai.zmq_pub_endpoint``), and finally falls back to
            ``tcp://127.0.0.1:5557``.
    """

    TOPIC_FACE_RESULT = b"face.result"
    TOPIC_HEARTBEAT = b"face.heartbeat"

    def __init__(self, endpoint: str | None = None):
        if zmq is None:
            raise ImportError(
                "pyzmq is required. Install with: pip install pyzmq"
            )
        self.endpoint = _resolve_endpoint(endpoint)
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PUB)
        self.socket.bind(self.endpoint)

    def send(self, result: RecognitionResult) -> None:
        """Send a recognition result to the C++ engine.

        Args:
            result: RecognitionResult containing detected/recognized faces.
        """
        payload = result.to_json().encode("utf-8")
        self.socket.send_multipart([self.TOPIC_FACE_RESULT, payload])

    def send_heartbeat(self, fps: float = 0.0, status: str = "running") -> None:
        """Send a heartbeat message.

        Args:
            fps: Current processing frames per second.
            status: Module status string.
        """
        heartbeat = Heartbeat(
            module="python_ai",
            status=status,
            fps=fps,
            timestamp_ms=int(time.time() * 1000),
        )
        payload = heartbeat.to_json().encode("utf-8")
        self.socket.send_multipart([self.TOPIC_HEARTBEAT, payload])

    def close(self) -> None:
        """Close the ZeroMQ socket and context."""
        self.socket.close()
        self.context.term()
