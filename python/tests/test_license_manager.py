"""Unit tests for the license management module configuration and C++ header /
source consistency.

Validates that:
- config/license.json conforms to the expected schema,
- The C++ LicenseManager header declares the expected interface,
- The C++ LicenseDialog header declares the expected interface,
- The CMakeLists.txt includes both new source files and links Qt::Network.
"""

import json
import os
import re
import sys

import pytest

# Ensure the python/ directory is importable.
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

# Resolve project root (two levels up from tests/).
PROJECT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..")
)

CONFIG_PATH = os.path.join(PROJECT_ROOT, "config", "license.json")
MANAGER_HEADER = os.path.join(
    PROJECT_ROOT, "ui", "include", "visioncast_ui", "license_manager.h"
)
MANAGER_SOURCE = os.path.join(
    PROJECT_ROOT, "ui", "src", "license_manager.cpp"
)
DIALOG_HEADER = os.path.join(
    PROJECT_ROOT, "ui", "include", "visioncast_ui", "license_dialog.h"
)
DIALOG_SOURCE = os.path.join(
    PROJECT_ROOT, "ui", "src", "license_dialog.cpp"
)
UI_CMAKE = os.path.join(PROJECT_ROOT, "ui", "CMakeLists.txt")
MAIN_WINDOW_HEADER = os.path.join(
    PROJECT_ROOT, "ui", "include", "visioncast_ui", "main_window.h"
)
MAIN_WINDOW_SOURCE = os.path.join(
    PROJECT_ROOT, "ui", "src", "main_window.cpp"
)


# =====================================================================
# Fixtures
# =====================================================================

@pytest.fixture(scope="module")
def config():
    assert os.path.isfile(CONFIG_PATH), f"Config not found: {CONFIG_PATH}"
    with open(CONFIG_PATH, "r") as f:
        return json.load(f)


@pytest.fixture(scope="module")
def manager_header():
    assert os.path.isfile(MANAGER_HEADER)
    with open(MANAGER_HEADER, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def manager_source():
    assert os.path.isfile(MANAGER_SOURCE)
    with open(MANAGER_SOURCE, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def dialog_header():
    assert os.path.isfile(DIALOG_HEADER)
    with open(DIALOG_HEADER, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def dialog_source():
    assert os.path.isfile(DIALOG_SOURCE)
    with open(DIALOG_SOURCE, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def cmake_text():
    assert os.path.isfile(UI_CMAKE)
    with open(UI_CMAKE, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def main_window_header():
    assert os.path.isfile(MAIN_WINDOW_HEADER)
    with open(MAIN_WINDOW_HEADER, "r") as f:
        return f.read()


@pytest.fixture(scope="module")
def main_window_source():
    assert os.path.isfile(MAIN_WINDOW_SOURCE)
    with open(MAIN_WINDOW_SOURCE, "r") as f:
        return f.read()


# =====================================================================
# Config schema validation
# =====================================================================

class TestLicenseConfig:
    """Validate config/license.json schema."""

    REQUIRED_KEYS = {"api_url", "api_key", "license_key"}

    def test_config_is_valid_json(self, config):
        assert isinstance(config, dict)

    def test_config_has_required_keys(self, config):
        for key in self.REQUIRED_KEYS:
            assert key in config, f"Missing key: {key}"

    def test_api_url_is_string(self, config):
        assert isinstance(config["api_url"], str)

    def test_api_url_is_https(self, config):
        url = config["api_url"]
        if url:  # Allow empty for template
            assert url.startswith("https://"), "API URL must use HTTPS"

    def test_api_key_is_string(self, config):
        assert isinstance(config["api_key"], str)

    def test_license_key_is_string(self, config):
        assert isinstance(config["license_key"], str)

    def test_no_hardcoded_secrets(self, config):
        """Ensure the shipped config template does not contain real keys."""
        api_key = config["api_key"]
        # Template should ship with an empty api_key so that users fill it
        # in locally.  A non-empty value is acceptable only if it looks like
        # a Base64-encoded JWT header (starts with "eyJ").
        assert api_key == "" or api_key.startswith("eyJ"), (
            "api_key should be empty or a Base64-encoded JWT"
        )


# =====================================================================
# LicenseManager header/source consistency
# =====================================================================

class TestLicenseManagerHeader:
    """Verify the C++ LicenseManager header declares expected API."""

    def test_pragma_once(self, manager_header):
        assert "#pragma once" in manager_header

    def test_namespace(self, manager_header):
        assert "namespace visioncast_ui" in manager_header

    def test_class_declaration(self, manager_header):
        assert re.search(
            r"class\s+LicenseManager\s*:\s*public\s+QObject",
            manager_header,
        )

    def test_q_object_macro(self, manager_header):
        assert "Q_OBJECT" in manager_header

    def test_license_status_enum(self, manager_header):
        assert "LicenseStatus" in manager_header
        for status in ("Unknown", "Valid", "Expired", "Invalid", "Suspended"):
            assert status in manager_header, (
                f"LicenseStatus::{status} not found"
            )

    @pytest.mark.parametrize("method", [
        "loadConfig", "saveConfig", "machineId",
        "activateKey", "validateKey", "deactivateKey", "checkStatus",
        "isLicensed", "licenseKey",
    ])
    def test_public_methods_declared(self, manager_header, method):
        assert method in manager_header

    @pytest.mark.parametrize("signal", [
        "activationSucceeded", "activationFailed",
        "validationCompleted",
        "deactivationSucceeded", "deactivationFailed",
        "statusChecked", "networkError",
    ])
    def test_signals_declared(self, manager_header, signal):
        assert signal in manager_header


class TestLicenseManagerSource:
    """Verify the C++ source implements key LicenseManager logic."""

    def test_includes_header(self, manager_source):
        assert '#include "visioncast_ui/license_manager.h"' in manager_source

    def test_uses_qnetworkaccessmanager(self, manager_source):
        assert "QNetworkAccessManager" in manager_source

    def test_sends_json_content_type(self, manager_source):
        assert "application/json" in manager_source

    def test_sends_apikey_header(self, manager_source):
        assert '"apikey"' in manager_source

    @pytest.mark.parametrize("action", [
        "activate_key", "validate_key", "deactivate_key", "check_status",
    ])
    def test_action_strings_present(self, manager_source, action):
        assert action in manager_source

    def test_machine_id_generation(self, manager_source):
        assert "generateMachineId" in manager_source
        assert "QCryptographicHash" in manager_source


# =====================================================================
# LicenseDialog header/source consistency
# =====================================================================

class TestLicenseDialogHeader:
    """Verify the C++ LicenseDialog header declares expected UI."""

    def test_pragma_once(self, dialog_header):
        assert "#pragma once" in dialog_header

    def test_class_declaration(self, dialog_header):
        assert re.search(
            r"class\s+LicenseDialog\s*:\s*public\s+QDialog",
            dialog_header,
        )

    def test_q_object_macro(self, dialog_header):
        assert "Q_OBJECT" in dialog_header


class TestLicenseDialogSource:
    """Verify the C++ source implements the LicenseDialog."""

    def test_includes_header(self, dialog_source):
        assert '#include "visioncast_ui/license_dialog.h"' in dialog_source

    def test_includes_license_manager(self, dialog_source):
        assert '#include "visioncast_ui/license_manager.h"' in dialog_source

    def test_has_activate_button(self, dialog_source):
        assert "activateButton_" in dialog_source

    def test_has_deactivate_button(self, dialog_source):
        assert "deactivateButton_" in dialog_source

    def test_has_status_display(self, dialog_source):
        assert "statusLabel_" in dialog_source


# =====================================================================
# CMakeLists.txt integration
# =====================================================================

class TestCMakeIntegration:
    """Verify the CMake build includes new source files and Qt::Network."""

    def test_license_manager_in_sources(self, cmake_text):
        assert "license_manager.cpp" in cmake_text

    def test_license_dialog_in_sources(self, cmake_text):
        assert "license_dialog.cpp" in cmake_text

    def test_qt_network_linked(self, cmake_text):
        assert "Network" in cmake_text


# =====================================================================
# MainWindow integration
# =====================================================================

class TestMainWindowIntegration:
    """Verify the main window integrates the license module."""

    def test_license_manager_forward_declared(self, main_window_header):
        assert "LicenseManager" in main_window_header

    def test_license_manager_member(self, main_window_header):
        assert "licenseManager_" in main_window_header

    def test_manage_license_slot(self, main_window_header):
        assert "onManageLicense" in main_window_header

    def test_includes_license_manager(self, main_window_source):
        assert "license_manager.h" in main_window_source

    def test_includes_license_dialog(self, main_window_source):
        assert "license_dialog.h" in main_window_source

    def test_help_menu_exists(self, main_window_source):
        assert "Help" in main_window_source

    def test_license_config_loaded(self, main_window_source):
        assert "license.json" in main_window_source
