import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .limits_config import (
    SETTLE_AFTER_WRITE_S,
    AT_MAX_SCENARIOS,
    PUMPS_LIMITS,
    OVER_MAX_FACTOR,
)


def _get_info_limit(scenario: dict):
    resp = requests.get(f"{BASE_URL}{scenario['info_url']}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, (
        f"GET {scenario['info_url']} returned {resp.status_code}"
    )
    return resp.json()[scenario["limit_field"]]


def _cleanup(scenario: dict):
    try:
        requests.get(f"{BASE_URL}{scenario['cleanup_url']}", timeout=REQUEST_TIMEOUT)
    except Exception:
        pass


# ==========================================
# Limits from info endpoints
# ==========================================

class TestAtMaxLimits:
    """Device value must not exceed the limit reported by its /info endpoint."""

    @pytest.mark.parametrize("scenario", AT_MAX_SCENARIOS, ids=lambda s: s["name"])
    def test_at_max_does_not_exceed_limit(self, scenario):
        try:
            limit = _get_info_limit(scenario)
            set_value = scenario["set_value"] if scenario["set_value"] is not None else limit

            resp = requests.post(
                f"{BASE_URL}{scenario['set_url']}",
                json={scenario["set_field"]: set_value},
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"POST {scenario['set_url']}={set_value} returned {resp.status_code}"
            )

            time.sleep(SETTLE_AFTER_WRITE_S)

            resp = requests.get(f"{BASE_URL}{scenario['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200, (
                f"GET {scenario['get_url']} returned {resp.status_code}"
            )
            actual = float(resp.json()[scenario["get_field"]])
            assert actual <= float(limit), (
                f"{scenario['name']}: actual {actual} exceeds limit {limit}"
            )
        finally:
            _cleanup(scenario)


class TestOverMaxClamped:
    """When set above the limit, the device must never report a value higher than the limit."""

    @pytest.mark.parametrize("scenario", AT_MAX_SCENARIOS, ids=lambda s: s["name"])
    def test_over_max_is_clamped(self, scenario):
        try:
            limit = float(_get_info_limit(scenario))
            factor = OVER_MAX_FACTOR
            over_value = limit * factor if limit != 0 else 1.0

            resp = requests.post(
                f"{BASE_URL}{scenario['set_url']}",
                json={scenario["set_field"]: over_value},
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"POST {scenario['set_url']}={over_value} returned {resp.status_code}"
            )

            time.sleep(SETTLE_AFTER_WRITE_S)

            resp = requests.get(f"{BASE_URL}{scenario['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200, (
                f"GET {scenario['get_url']} returned {resp.status_code}"
            )
            actual = float(resp.json()[scenario["get_field"]])
            assert actual <= limit, (
                f"{scenario['name']}: set to {over_value} but device reports {actual}, which exceeds limit {limit}"
            )
        finally:
            _cleanup(scenario)


class TestPumpsMaxFlowrate:
    """Pump flowrate must never exceed the limit reported by /pumps/{inst}/info/{pump}."""

    def _run_for_all_pumps(self, pump_instances, set_flowrate_fn):
        if not pump_instances:
            pytest.skip("No pump instances discovered")
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                info_resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/info/{pump}", timeout=REQUEST_TIMEOUT
                )
                assert info_resp.status_code == 200, (
                    f"GET /pumps/{inst}/info/{pump} returned {info_resp.status_code}"
                )
                max_flowrate = float(info_resp.json()["max_flowrate"])
                flowrate = set_flowrate_fn(max_flowrate)

                post_resp = requests.post(
                    f"{BASE_URL}/pumps/{inst}/flowrate/{pump}",
                    json={"flowrate": flowrate},
                    timeout=REQUEST_TIMEOUT,
                )
                assert post_resp.status_code == 200, (
                    f"POST /pumps/{inst}/flowrate/{pump}={flowrate} returned {post_resp.status_code}"
                )

                time.sleep(SETTLE_AFTER_WRITE_S)

                get_resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/flowrate/{pump}", timeout=REQUEST_TIMEOUT
                )
                assert get_resp.status_code == 200
                actual = float(get_resp.json()["flowrate"])
                assert abs(actual) <= abs(max_flowrate), (
                    f"pumps/{inst}/{pump}: set to {flowrate}, actual {actual} exceeds max_flowrate {max_flowrate}"
                )

                requests.get(
                    f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT
                )

    def test_at_max_flowrate_does_not_exceed_limit(self, pump_instances):
        """Set flowrate to max and verify actual value stays within limit."""
        self._run_for_all_pumps(pump_instances, lambda max_f: max_f)

    def test_over_max_flowrate_is_clamped(self, pump_instances):
        """Set flowrate above max and verify device never reports more than the limit."""
        factor = PUMPS_LIMITS["over_max_factor"]
        self._run_for_all_pumps(pump_instances, lambda max_f: max_f * factor if max_f != 0 else 1.0)
