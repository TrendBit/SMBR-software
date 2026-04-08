import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .move_config import (
    MOVE_SETTLE_S,
    MOVE_COMPLETE_SCENARIOS,
    MOVE_INTERRUPT_SCENARIOS,
    STIR_COMPLETE_SCENARIOS,
    PUMPS_MOVE_BODY,
)


class TestMoveCompletes:
    """A move command must be in-progress immediately after POST, and zero after completion."""

    @pytest.mark.parametrize("scenario", MOVE_COMPLETE_SCENARIOS, ids=lambda s: s["name"])
    def test_in_progress_then_complete(self, scenario):
        try:
            resp = requests.post(
                f"{BASE_URL}{scenario['move_url']}",
                json=scenario["move_body"],
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"POST {scenario['move_url']} returned {resp.status_code}"
            )

            time.sleep(MOVE_SETTLE_S)

            resp = requests.get(f"{BASE_URL}{scenario['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200
            actual = float(resp.json()[scenario["get_field"]])
            assert actual != 0.0, (
                f"{scenario['name']}: expected motion in progress (flowrate ≠ 0), got {actual}"
            )

            time.sleep(scenario["expected_duration_s"])

            resp = requests.get(f"{BASE_URL}{scenario['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200
            actual = float(resp.json()[scenario["get_field"]])
            assert actual == 0.0, (
                f"{scenario['name']}: expected motion complete (flowrate = 0), got {actual}"
            )
        finally:
            requests.get(f"{BASE_URL}{scenario['stop_url']}", timeout=REQUEST_TIMEOUT)


class TestMoveInterrupted:
    """A stop command must immediately halt motion (flowrate returns to 0)."""

    @pytest.mark.parametrize("scenario", MOVE_INTERRUPT_SCENARIOS, ids=lambda s: s["name"])
    def test_stop_interrupts_move(self, scenario):
        try:
            resp = requests.post(
                f"{BASE_URL}{scenario['move_url']}",
                json=scenario["move_body"],
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"POST {scenario['move_url']} returned {resp.status_code}"
            )

            requests.get(f"{BASE_URL}{scenario['stop_url']}", timeout=REQUEST_TIMEOUT)

            time.sleep(MOVE_SETTLE_S)

            resp = requests.get(f"{BASE_URL}{scenario['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200
            actual = float(resp.json()[scenario["get_field"]])
            assert actual == 0.0, (
                f"{scenario['name']}: expected stop to interrupt move (flowrate = 0), got {actual}"
            )
        finally:
            requests.get(f"{BASE_URL}{scenario['stop_url']}", timeout=REQUEST_TIMEOUT)


class TestStirCompletes:
    """Mixer stir with a fixed duration must return to speed=0 after the time elapses."""

    @pytest.mark.parametrize("scenario", STIR_COMPLETE_SCENARIOS, ids=lambda s: s["name"])
    def test_stir_completes_after_duration(self, scenario):
        try:
            resp = requests.post(
                f"{BASE_URL}{scenario['stir_url']}",
                json=scenario["stir_body"],
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, (
                f"POST {scenario['stir_url']} returned {resp.status_code}"
            )

            time.sleep(scenario["wait_s"])

            resp = requests.get(f"{BASE_URL}{scenario['get_url']}", timeout=REQUEST_TIMEOUT)
            assert resp.status_code == 200
            actual = float(resp.json()[scenario["get_field"]])
            assert actual == 0.0, (
                f"{scenario['name']}: expected speed=0 after stir duration, got {actual}"
            )
        finally:
            requests.get(f"{BASE_URL}{scenario['stop_url']}", timeout=REQUEST_TIMEOUT)


class TestPumpsMoveInterrupted:
    """A stop command must immediately halt pump motion (flowrate returns to 0)."""

    def test_stop_interrupts_move(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances discovered")
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                try:
                    resp = requests.post(
                        f"{BASE_URL}/pumps/{inst}/move/{pump}",
                        json=PUMPS_MOVE_BODY,
                        timeout=REQUEST_TIMEOUT,
                    )
                    assert resp.status_code == 200, (
                        f"POST /pumps/{inst}/move/{pump} returned {resp.status_code}"
                    )

                    requests.get(
                        f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT
                    )

                    time.sleep(MOVE_SETTLE_S)

                    resp = requests.get(
                        f"{BASE_URL}/pumps/{inst}/flowrate/{pump}", timeout=REQUEST_TIMEOUT
                    )
                    assert resp.status_code == 200
                    actual = float(resp.json()["flowrate"])
                    assert actual == 0.0, (
                        f"pumps/{inst}/{pump}: expected stop to interrupt move (flowrate = 0), got {actual}"
                    )
                finally:
                    requests.get(
                        f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT
                    )
