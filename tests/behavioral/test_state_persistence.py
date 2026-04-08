import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .state_persistence_config import (
    CONTROL_MODULE,
    RESTART_SETTLE_S,
    STATE_PERSISTENCE_SCENARIOS,
)


def _setup(steps: list):
    for step in steps:
        body = {step["field"]: step["value"]}
        resp = requests.post(f"{BASE_URL}{step['url']}", json=body, timeout=REQUEST_TIMEOUT)
        assert resp.status_code == 200, (
            f"Setup POST {step['url']} = {step['value']} returned {resp.status_code}"
        )


def _get_module_uid(module_type: str) -> str:
    resp = requests.get(f"{BASE_URL}/system/modules", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, f"GET /system/modules returned {resp.status_code}"
    for entry in resp.json():
        if entry.get("module_type") == module_type:
            return entry["uid"]
    raise ValueError(f"Module '{module_type}' not found in /system/modules")


def _restart_module(module: str):
    uid = _get_module_uid(module)
    resp = requests.post(
        f"{BASE_URL}/{module}/restart",
        json={"uid": uid},
        timeout=REQUEST_TIMEOUT,
    )
    assert resp.status_code == 200, f"POST /{module}/restart returned {resp.status_code}"


def _verify(checks: list):
    for check in checks:
        resp = requests.get(f"{BASE_URL}{check['url']}", timeout=REQUEST_TIMEOUT)
        assert resp.status_code == 200, (
            f"Verify GET {check['url']} returned {resp.status_code}"
        )
        actual = float(resp.json()[check["field"]])
        assert actual == pytest.approx(check["expected"], abs=check["tolerance"]), (
            f"After restart: {check['url']} expected {check['expected']}, got {actual}"
        )


def _cleanup(steps: list):
    for step in steps:
        try:
            if step["type"] == "get":
                requests.get(f"{BASE_URL}{step['url']}", timeout=REQUEST_TIMEOUT)
            else:
                requests.post(f"{BASE_URL}{step['url']}", json=step["body"], timeout=REQUEST_TIMEOUT)
        except Exception:
            pass


# ==========================================
# State persistence after module restart
# ==========================================

class TestStatePersistenceAfterRestart:
    """Verify that configured values survive a control module restart."""

    @pytest.mark.parametrize("scenario", STATE_PERSISTENCE_SCENARIOS, ids=lambda s: s["name"])
    def test_state_persists_after_restart(self, scenario):
        try:
            _setup(scenario["setup"])
            _restart_module(CONTROL_MODULE)
            time.sleep(RESTART_SETTLE_S)
            _verify(scenario["verify"])
        finally:
            _cleanup(scenario["cleanup"])
