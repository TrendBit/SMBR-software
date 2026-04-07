import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .power_draw_config import (
    POWER_DRAW_URL,
    BASELINE_STABILIZE_S,
    BASELINE_SAMPLES,
    POWER_DRAW_ENDPOINTS,
    PUMPS_ACTIVATION_SPEED,
    PUMPS_SETTLE_TIME_S,
    PUMPS_MIN_INCREASE_W,
)


def _get_power_draw() -> float:
    """Helper: fetch current power draw value (Watts)."""
    resp = requests.get(f"{BASE_URL}{POWER_DRAW_URL}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, f"GET {POWER_DRAW_URL} returned {resp.status_code}"
    return float(resp.json()["power_draw"])


def _get_stable_baseline() -> float:
    """Wait for system to stabilise, then return an averaged baseline power draw."""
    time.sleep(BASELINE_STABILIZE_S)
    samples = []
    for _ in range(BASELINE_SAMPLES):
        samples.append(_get_power_draw())
        time.sleep(0.5)
    baseline = sum(samples) / len(samples)
    return baseline


def _cleanup_endpoint(endpoint: dict):
    """Stop / zero out a single endpoint after a test."""
    if endpoint.get("stop_url"):
        try:
            requests.get(f"{BASE_URL}{endpoint['stop_url']}", timeout=REQUEST_TIMEOUT)
        except Exception:
            pass
    if endpoint.get("cleanup_post"):
        cp = endpoint["cleanup_post"]
        try:
            requests.post(
                f"{BASE_URL}{cp['post_url']}",
                json={cp["field"]: cp["value"]},
                timeout=REQUEST_TIMEOUT,
            )
        except Exception:
            pass


# ==========================================
# Per-actuator power draw tests
# ==========================================

class TestPowerDrawPerActuator:
    """Enabling each actuator individually must raise total power draw above baseline."""

    @pytest.fixture(autouse=True)
    def stop_after(self, request):
        yield
        endpoint = request.node.callspec.params.get("endpoint", {})
        _cleanup_endpoint(endpoint)

    @pytest.mark.parametrize("endpoint", POWER_DRAW_ENDPOINTS, ids=lambda e: e["name"])
    def test_power_increases_when_active(self, endpoint):
        baseline = _get_stable_baseline()

        for act in endpoint["activate"]:
            resp = requests.post(
                f"{BASE_URL}{act['post_url']}",
                json={act["field"]: act["value"]},
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"{endpoint['name']}: activation POST {act['post_url']} "
                f"value={act['value']} returned {resp.status_code}"
            )

        time.sleep(endpoint.get("settle_time", 2.0))

        after = _get_power_draw()

        increase = after - baseline
        assert after > baseline + endpoint["min_increase_w"], (
            f"{endpoint['name']}: expected power draw to increase by at least "
            f"{endpoint['min_increase_w']} W above baseline {baseline:.2f} W, "
            f"got {after:.2f} W (increase={increase:+.3f} W)"
        )


# ==========================================
# Pump module power draw
# ==========================================

class TestPumpsModulePowerDraw:
    """Running pump module peristaltic pumps must raise total power draw."""

    @pytest.fixture(autouse=True)
    def stop_pumps_after(self, pump_instances):
        yield
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                try:
                    requests.get(
                        f"{BASE_URL}/pumps/{inst}/stop/{pump}",
                        timeout=REQUEST_TIMEOUT,
                    )
                except Exception:
                    pass

    def test_power_increases_when_pumps_active(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")

        baseline = _get_stable_baseline()

        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                resp = requests.post(
                    f"{BASE_URL}/pumps/{inst}/speed/{pump}",
                    json={"speed": PUMPS_ACTIVATION_SPEED},
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200, (
                    f"activation POST speed={PUMPS_ACTIVATION_SPEED} failed "
                    f"for pump {inst}/{pump}: {resp.status_code}"
                )

        time.sleep(PUMPS_SETTLE_TIME_S)

        after = _get_power_draw()

        total_instance_count = len(pump_instances)
        expected_min_increase = total_instance_count * PUMPS_MIN_INCREASE_W
        increase = after - baseline

        assert after > baseline + expected_min_increase, (
            f"expected pump module power draw to increase by at least "
            f"{expected_min_increase:.2f} W above baseline {baseline:.2f} W "
            f"({total_instance_count} instance(s) × {PUMPS_MIN_INCREASE_W} W), "
            f"got {after:.2f} W (increase={increase:+.3f} W)"
        )
