"""Shared ZMQ endpoint resolution for Python IPC modules.

Endpoint resolution order (highest to lowest priority):
  1. Explicit argument (non-None value).
  2. ``ZMQ_PUB_ENDPOINT`` environment variable.
  3. ``ai.zmq_pub_endpoint`` key in ``config/system.json`` (located relative
     to this file's package root).
  4. Built-in default ``tcp://127.0.0.1:5557``.
"""

import json
import os

#: Built-in default ZeroMQ PUB endpoint (Python → C++ engine).
DEFAULT_ZMQ_PUB_ENDPOINT = "tcp://127.0.0.1:5557"

# Path to config/system.json relative to the project root
# (two levels up from this file which lives in python/ipc/).
_CONFIG_PATH = os.path.normpath(
    os.path.join(os.path.dirname(__file__), "..", "..", "config", "system.json")
)


def resolve_zmq_pub_endpoint(explicit: str | None = None) -> str:
    """Return the resolved ZMQ PUB endpoint.

    Args:
        explicit: When provided (non-None), this value is returned immediately,
            bypassing all other resolution steps.

    Returns:
        The resolved ZeroMQ PUB endpoint string.
    """
    if explicit is not None:
        return explicit
    env_val = os.environ.get("ZMQ_PUB_ENDPOINT")
    if env_val:
        return env_val
    try:
        with open(_CONFIG_PATH, encoding="utf-8") as fh:
            cfg = json.load(fh)
        ep = cfg.get("ai", {}).get("zmq_pub_endpoint")
        if ep:
            return ep
    except (OSError, json.JSONDecodeError, KeyError):
        pass
    return DEFAULT_ZMQ_PUB_ENDPOINT
