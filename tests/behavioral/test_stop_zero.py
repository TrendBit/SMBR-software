import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .stop_zero_config import STOP_ZERO_ENDPOINTS, PUMPS_ACTIVATION_SPEED


# ==========================================
# Control endpoints (heater, pumps, aerator, mixer)
# ==========================================

class TestStopZero:
    """After stopping an actuator its output-related GET endpoints must report zero."""

    @pytest.mark.parametrize("endpoint", STOP_ZERO_ENDPOINTS, ids=lambda e: e["name"])
    def test_stop_zero(self, endpoint):
        for act in endpoint.get("activate", []):
            resp = requests.post(
                f"{BASE_URL}{act['post_url']}",
                json={act["field"]: act["value"]},
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"{endpoint['name']}: activation POST {act['post_url']} "
                f"value={act['value']} returned {resp.status_code}"
            )

        resp = requests.get(f"{BASE_URL}{endpoint['stop_url']}", timeout=REQUEST_TIMEOUT)
        assert resp.status_code == 200, (
            f"{endpoint['name']}: stop GET {endpoint['stop_url']} returned {resp.status_code}"
        )

        settle = endpoint.get("settle_time", 0)
        if settle:
            time.sleep(settle)

        for v in endpoint["verify"]:
            resp = requests.get(f"{BASE_URL}{v['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200, (
                f"{endpoint['name']}: verify GET {v['get_url']} returned {resp.status_code}"
            )

            actual = resp.json().get(v["field"])
            expected = v["expected"]

            if expected is None:
                assert actual is None, (
                    f"{endpoint['name']}: {v['field']} expected null, got {actual!r}"
                )
            else:
                tol = v.get("tolerance", 0.01)
                assert actual == pytest.approx(expected, abs=tol), (
                    f"{endpoint['name']}: {v['field']} expected {expected}, got {actual}"
                )


# ==========================================
# Pumps module
# ==========================================

class TestPumpsStopZero:
    """After stopping a pump both speed and flowrate must be zero."""

    def test_stop_zeroes_speed_and_flowrate(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")

        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                resp = requests.post(
                    f"{BASE_URL}/pumps/{inst}/speed/{pump}",
                    json={"speed": PUMPS_ACTIVATION_SPEED},
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200, (
                    f"activation POST speed failed for pump {inst}/{pump}: {resp.status_code}"
                )

                resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/stop/{pump}",
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200, (
                    f"stop GET failed for pump {inst}/{pump}: {resp.status_code}"
                )

                resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/speed/{pump}",
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200
                assert resp.json()["speed"] == pytest.approx(0.0, abs=0.01), (
                    f"speed not zero after stop for pump {inst}/{pump}: {resp.json()['speed']}"
                )

                resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/flowrate/{pump}",
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200
                assert resp.json()["flowrate"] == pytest.approx(0.0, abs=0.1), (
                    f"flowrate not zero after stop for pump {inst}/{pump}: {resp.json()['flowrate']}"
                )
