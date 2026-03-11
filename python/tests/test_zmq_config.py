"""Unit tests for ZMQ endpoint configuration loading.

Validates that ZmqSender and MetadataSender resolve their endpoints in the
correct order: explicit argument > ZMQ_PUB_ENDPOINT env var >
config/system.json (ai.zmq_pub_endpoint) > built-in default
tcp://127.0.0.1:5557.

Security tests validate that non-loopback endpoints are rejected.

These tests run without pyzmq or any external ZMQ library.
"""

import json
import os
import sys
from unittest import mock

import pytest

# Ensure the python/ directory is importable.
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

# Resolve project root (two levels up from tests/).
PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)

_DEFAULT_ENDPOINT = "tcp://127.0.0.1:5557"


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _make_mock_zmq():
    """Return a mock zmq module that records socket operations."""
    mock_zmq = mock.MagicMock()
    mock_ctx = mock.MagicMock()
    mock_socket = mock.MagicMock()
    mock_zmq.Context.return_value = mock_ctx
    mock_ctx.socket.return_value = mock_socket
    mock_zmq.PUB = 1
    return mock_zmq, mock_socket


# ---------------------------------------------------------------------------
# _resolve_endpoint tests (ZmqSender)
# ---------------------------------------------------------------------------

class TestResolveEndpointZmqSender:
    """Tests for ipc.zmq_sender._resolve_endpoint."""

    def test_explicit_argument_wins(self):
        import ipc.zmq_sender as mod
        ep = mod._resolve_endpoint("tcp://127.0.0.1:9001")
        assert ep == "tcp://127.0.0.1:9001"

    def test_env_var_wins_over_config(self, tmp_path, monkeypatch):
        import ipc.zmq_sender as mod
        # Write a config that would return a different endpoint.
        cfg = {"ai": {"zmq_pub_endpoint": "tcp://127.0.0.1:9002"}}
        config_file = tmp_path / "system.json"
        config_file.write_text(json.dumps(cfg))
        monkeypatch.setenv("ZMQ_PUB_ENDPOINT", "tcp://127.0.0.1:9003")
        ep = mod._resolve_endpoint(None)
        assert ep == "tcp://127.0.0.1:9003"

    def test_config_file_used_when_no_env(self, monkeypatch):
        import ipc._config as cfg_mod
        monkeypatch.delenv("ZMQ_PUB_ENDPOINT", raising=False)
        cfg_json = json.dumps({"ai": {"zmq_pub_endpoint": "tcp://127.0.0.1:9004"}})
        # Simulate a readable config/system.json by patching builtins.open only
        # for the config path lookup inside resolve_zmq_pub_endpoint.
        real_open = open

        def fake_open(path, *args, **kwargs):
            if str(path).endswith("system.json"):
                import io
                return io.StringIO(cfg_json)
            return real_open(path, *args, **kwargs)

        with mock.patch("builtins.open", side_effect=fake_open):
            ep = cfg_mod.resolve_zmq_pub_endpoint(None)
        assert ep == "tcp://127.0.0.1:9004"

    def test_falls_back_to_default(self, monkeypatch):
        import ipc._config as cfg_mod
        monkeypatch.delenv("ZMQ_PUB_ENDPOINT", raising=False)
        # Simulate config/system.json being absent.
        with mock.patch("builtins.open", side_effect=OSError("not found")):
            ep = cfg_mod.resolve_zmq_pub_endpoint(None)
        assert ep == _DEFAULT_ENDPOINT

    def test_default_endpoint_constant(self):
        import ipc._config as cfg_mod
        assert cfg_mod.DEFAULT_ZMQ_PUB_ENDPOINT == _DEFAULT_ENDPOINT


# ---------------------------------------------------------------------------
# _resolve_endpoint tests (MetadataSender)
# ---------------------------------------------------------------------------

class TestResolveEndpointMetadataSender:
    """Tests for ipc.metadata_sender._resolve_endpoint (delegates to ipc._config)."""

    def test_explicit_argument_wins(self):
        import ipc.metadata_sender as mod
        ep = mod._resolve_endpoint("tcp://127.0.0.1:9011")
        assert ep == "tcp://127.0.0.1:9011"

    def test_falls_back_to_default(self, monkeypatch):
        import ipc._config as cfg_mod
        monkeypatch.delenv("ZMQ_PUB_ENDPOINT", raising=False)
        with mock.patch("builtins.open", side_effect=OSError("not found")):
            ep = cfg_mod.resolve_zmq_pub_endpoint(None)
        assert ep == _DEFAULT_ENDPOINT

    def test_default_endpoint_constant(self):
        import ipc._config as cfg_mod
        assert cfg_mod.DEFAULT_ZMQ_PUB_ENDPOINT == _DEFAULT_ENDPOINT


# ---------------------------------------------------------------------------
# ZmqSender constructor endpoint resolution
# ---------------------------------------------------------------------------

class TestZmqSenderEndpointResolution:
    """Verify ZmqSender stores and binds the resolved endpoint."""

    @mock.patch("ipc.zmq_sender.zmq")
    def test_explicit_endpoint_is_stored(self, mock_zmq):
        mock_zmq.Context.return_value = mock.MagicMock()
        mock_zmq.Context.return_value.socket.return_value = mock.MagicMock()
        mock_zmq.PUB = 1
        from ipc.zmq_sender import ZmqSender
        sender = ZmqSender(endpoint="tcp://127.0.0.1:9021")
        assert sender.endpoint == "tcp://127.0.0.1:9021"

    @mock.patch("ipc.zmq_sender.zmq")
    def test_default_is_5557(self, mock_zmq, monkeypatch):
        mock_zmq.Context.return_value = mock.MagicMock()
        mock_zmq.Context.return_value.socket.return_value = mock.MagicMock()
        mock_zmq.PUB = 1
        monkeypatch.delenv("ZMQ_PUB_ENDPOINT", raising=False)
        from ipc.zmq_sender import ZmqSender
        with mock.patch("builtins.open", side_effect=OSError("not found")):
            sender = ZmqSender()
        assert sender.endpoint == _DEFAULT_ENDPOINT

    @mock.patch("ipc.zmq_sender.zmq")
    def test_env_var_overrides_default(self, mock_zmq, monkeypatch):
        mock_zmq.Context.return_value = mock.MagicMock()
        mock_zmq.Context.return_value.socket.return_value = mock.MagicMock()
        mock_zmq.PUB = 1
        monkeypatch.setenv("ZMQ_PUB_ENDPOINT", "tcp://127.0.0.1:9022")
        from ipc.zmq_sender import ZmqSender
        sender = ZmqSender()
        assert sender.endpoint == "tcp://127.0.0.1:9022"


# ---------------------------------------------------------------------------
# config/system.json uses zmq_pub_endpoint key and port 5557
# ---------------------------------------------------------------------------

class TestSystemJsonConfig:
    """Validate the canonical config/system.json matches expected values."""

    def test_zmq_pub_endpoint_key_present(self):
        config_path = os.path.join(PROJECT_ROOT, "config", "system.json")
        with open(config_path, encoding="utf-8") as fh:
            cfg = json.load(fh)
        assert "zmq_pub_endpoint" in cfg["ai"], (
            "config/system.json must have ai.zmq_pub_endpoint key"
        )

    def test_zmq_pub_endpoint_value(self):
        config_path = os.path.join(PROJECT_ROOT, "config", "system.json")
        with open(config_path, encoding="utf-8") as fh:
            cfg = json.load(fh)
        assert cfg["ai"]["zmq_pub_endpoint"] == _DEFAULT_ENDPOINT, (
            f"ai.zmq_pub_endpoint must be {_DEFAULT_ENDPOINT}"
        )

    def test_old_zmq_endpoint_key_absent(self):
        config_path = os.path.join(PROJECT_ROOT, "config", "system.json")
        with open(config_path, encoding="utf-8") as fh:
            cfg = json.load(fh)
        assert "zmq_endpoint" not in cfg.get("ai", {}), (
            "Deprecated ai.zmq_endpoint key should not be present; "
            "use ai.zmq_pub_endpoint instead."
        )


# ---------------------------------------------------------------------------
# Security: Loopback endpoint validation
# ---------------------------------------------------------------------------

class TestLoopbackValidation:
    """Validate that non-loopback endpoints are rejected for security."""

    def test_validate_loopback_127_0_0_1(self):
        """127.0.0.1 should be accepted as loopback."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("tcp://127.0.0.1:5557") is True

    def test_validate_loopback_localhost(self):
        """localhost should be accepted as loopback."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("tcp://localhost:5557") is True

    def test_validate_loopback_ipv6(self):
        """IPv6 loopback ([::1]) should be accepted."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("tcp://[::1]:5557") is True

    def test_validate_loopback_ipc(self):
        """IPC endpoints should always be accepted (local by nature)."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("ipc:///tmp/zmq.sock") is True

    def test_reject_external_ip(self):
        """External IP addresses should be rejected."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("tcp://192.168.1.1:5557") is False

    def test_reject_0_0_0_0(self):
        """0.0.0.0 (all interfaces) should be rejected."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("tcp://0.0.0.0:5557") is False

    def test_reject_public_ip(self):
        """Public IP addresses should be rejected."""
        import ipc._config as cfg_mod
        assert cfg_mod.validate_loopback_endpoint("tcp://8.8.8.8:5557") is False

    def test_resolve_rejects_non_loopback_explicit(self, monkeypatch):
        """Non-loopback explicit endpoint should raise ValueError."""
        import ipc._config as cfg_mod
        monkeypatch.delenv("ZMQ_PUB_ENDPOINT", raising=False)
        with pytest.raises(ValueError, match="not a loopback address"):
            cfg_mod.resolve_zmq_pub_endpoint("tcp://192.168.1.100:5557")

    def test_resolve_rejects_non_loopback_env(self, monkeypatch):
        """Non-loopback endpoint from env var should raise ValueError."""
        import ipc._config as cfg_mod
        monkeypatch.setenv("ZMQ_PUB_ENDPOINT", "tcp://10.0.0.1:5557")
        with mock.patch("builtins.open", side_effect=OSError("not found")):
            with pytest.raises(ValueError, match="not a loopback address"):
                cfg_mod.resolve_zmq_pub_endpoint(None)

    def test_invalid_endpoint_format(self):
        """Invalid endpoint format should raise ValueError with helpful message."""
        import ipc._config as cfg_mod
        with pytest.raises(
            ValueError,
            match=r"Invalid endpoint format.*Expected tcp://host:port or ipc://path"
        ):
            cfg_mod.validate_loopback_endpoint("invalid://format")
