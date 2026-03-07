"""Pytest configuration and shared fixtures for VisionCast-AI Python tests."""

import os
import sys
from unittest import mock

import pytest

# Ensure the python/ directory is on sys.path so test modules can import
# ai, ipc, and monitoring packages without installation.
_PYTHON_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if _PYTHON_DIR not in sys.path:
    sys.path.insert(0, _PYTHON_DIR)


@pytest.fixture(autouse=False)
def mock_face_recognition():
    """Provide a lightweight mock for the *face_recognition* library.

    Yields the mock object so individual tests can configure return values.
    This fixture avoids requiring dlib/CUDA in CI environments.
    """
    with mock.patch.dict("sys.modules", {"face_recognition": mock.MagicMock()}):
        import face_recognition as fr_mock
        yield fr_mock
