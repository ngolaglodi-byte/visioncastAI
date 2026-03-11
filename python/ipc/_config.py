"""Shared ZMQ endpoint resolution for Python IPC modules.

Endpoint resolution order (highest to lowest priority):
  1. Explicit argument (non-None value).
  2. ``ZMQ_PUB_ENDPOINT`` environment variable.
  3. ``ai.zmq_pub_endpoint`` key in ``config/system.json`` (located relative
     to this file's package root).
  4. Built-in default ``tcp://127.0.0.1:5557``.

Security:
  All endpoints are validated to use loopback addresses (127.0.0.1 or localhost)
  to prevent accidental network exposure. Non-loopback endpoints raise a
  ``ValueError``.
"""

import json
import logging
import os
import re

#: Built-in default ZeroMQ PUB endpoint (Python → C++ engine).
DEFAULT_ZMQ_PUB_ENDPOINT = "tcp://127.0.0.1:5557"

#: Allowed loopback hosts for ZeroMQ endpoints (security constraint).
_LOOPBACK_HOSTS = frozenset({"127.0.0.1", "localhost", "::1", "[::1]"})

#: Regex to extract host from tcp://host:port (supports IPv4, hostname, and bracketed IPv6)
#: For IPv6, host must be in brackets like [::1]:port
_ENDPOINT_PATTERN = re.compile(r"^tcp://(\[[^\]]+\]|[^:]+):(\d+)$")

# Path to config/system.json relative to the project root
# (two levels up from this file which lives in python/ipc/).
_CONFIG_PATH = os.path.normpath(
    os.path.join(os.path.dirname(__file__), "..", "..", "config", "system.json")
)

_logger = logging.getLogger(__name__)


def validate_loopback_endpoint(endpoint: str) -> bool:
    """Check if the endpoint uses a loopback address.

    Args:
        endpoint: ZeroMQ endpoint string (e.g., "tcp://127.0.0.1:5557").

    Returns:
        True if the endpoint uses a loopback address, False otherwise.

    Raises:
        ValueError: If the endpoint format is invalid.
    """
    # Allow IPC endpoints (ipc://) which are always local
    if endpoint.startswith("ipc://"):
        return True

    match = _ENDPOINT_PATTERN.match(endpoint)
    if not match:
        raise ValueError(
            f"Invalid endpoint format: {endpoint}. "
            "Expected tcp://host:port or ipc://path."
        )
    host = match.group(1).lower()
    return host in _LOOPBACK_HOSTS


def resolve_zmq_pub_endpoint(explicit: str | None = None) -> str:
    """Return the resolved ZMQ PUB endpoint.

    Args:
        explicit: When provided (non-None), this value is returned immediately,
            bypassing all other resolution steps.

    Returns:
        The resolved ZeroMQ PUB endpoint string.

    Raises:
        ValueError: If the resolved endpoint is not a loopback address
            (security constraint to prevent network exposure).
    """
    endpoint = None

    if explicit is not None:
        endpoint = explicit
    else:
        env_val = os.environ.get("ZMQ_PUB_ENDPOINT")
        if env_val:
            endpoint = env_val
        else:
            try:
                with open(_CONFIG_PATH, encoding="utf-8") as fh:
                    cfg = json.load(fh)
                ep = cfg.get("ai", {}).get("zmq_pub_endpoint")
                if ep:
                    endpoint = ep
            except (OSError, json.JSONDecodeError, KeyError):
                pass

    if endpoint is None:
        endpoint = DEFAULT_ZMQ_PUB_ENDPOINT

    # Security: Validate loopback-only binding
    if not validate_loopback_endpoint(endpoint):
        _logger.error(
            "Security: Non-loopback ZMQ endpoint rejected: %s. "
            "Only loopback addresses (127.0.0.1, localhost) are allowed.",
            endpoint,
        )
        raise ValueError(
            f"Security: ZMQ endpoint '{endpoint}' is not a loopback address. "
            "Only loopback addresses (127.0.0.1, localhost, ::1) are allowed "
            "to prevent network exposure. See SECURITY.md for details."
        )

    return endpoint
