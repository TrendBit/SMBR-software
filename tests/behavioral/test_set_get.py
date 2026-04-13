import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .set_get_config import (
    SET_GET_ENDPOINTS,
    LED_CHANNELS,
    LED_SINGLE_CHANNEL_VALUES,
    LED_ALL_CHANNEL_VALUES,
    PUMPS_SPEED_VALUES,
)

_STOP_URLS = [
    "/control/heater/turn_off",
    "/control/cuvette_pump/stop",
    "/control/aerator/stop",
    "/control/mixer/stop",
]


@pytest.fixture(autouse=True)
def stop_actuators_after_test():
    yield
    for url in _STOP_URLS:
        try:
            requests.get(f"{BASE_URL}{url}", timeout=REQUEST_TIMEOUT)
        except Exception:
            pass


# ==========================================
# LED Panel
# ==========================================

class TestLedPanelSetGet:

    @pytest.fixture(autouse=True)
    def turn_off_led_after_test(self):
        yield
        try:
            requests.post(
                f"{BASE_URL}/control/led_panel/intensity",
                json={"intensity": [0.0, 0.0, 0.0, 0.0]},
                timeout=REQUEST_TIMEOUT,
            )
        except Exception:
            pass

    @pytest.mark.parametrize("channel", LED_CHANNELS)
    @pytest.mark.parametrize("value", LED_SINGLE_CHANNEL_VALUES)
    def test_single_channel_set_get(self, channel, value):
        resp = requests.post(
            f"{BASE_URL}/control/led_panel/intensity/{channel}",
            json={"intensity": value},
            timeout=REQUEST_TIMEOUT,
        )
        assert resp.status_code == 200

        resp = requests.get(
            f"{BASE_URL}/control/led_panel/intensity/{channel}",
            timeout=REQUEST_TIMEOUT,
        )
        assert resp.status_code == 200
        assert resp.json()["intensity"] == pytest.approx(value, abs=0.01)

    @pytest.mark.parametrize("values", LED_ALL_CHANNEL_VALUES)
    def test_all_channels_set_get(self, values):
        resp = requests.post(
            f"{BASE_URL}/control/led_panel/intensity",
            json={"intensity": values},
            timeout=REQUEST_TIMEOUT,
        )
        assert resp.status_code == 200

        for channel, expected in zip(LED_CHANNELS, values):
            resp = requests.get(
                f"{BASE_URL}/control/led_panel/intensity/{channel}",
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200
            assert resp.json()["intensity"] == pytest.approx(expected, abs=0.01), \
                f"channel {channel}: expected {expected}, got {resp.json()['intensity']}"


# ==========================================
# Heater, Cuvette Pump, Aerator, Mixer
# ==========================================

class TestSimpleSetGet:
    @pytest.mark.parametrize("endpoint", SET_GET_ENDPOINTS, ids=lambda e: e["name"])
    def test_set_get(self, endpoint):
        for value in endpoint["values"]:
            resp = requests.post(
                f"{BASE_URL}{endpoint['post_url']}",
                json={endpoint["field"]: value},
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, \
                f"{endpoint['name']}: POST {value} returned {resp.status_code}"

            time.sleep(endpoint.get("settle_time", 0))

            resp = requests.get(
                f"{BASE_URL}{endpoint['get_url']}",
                timeout=REQUEST_TIMEOUT,
            )
            assert resp.status_code == 200, \
                f"{endpoint['name']}: GET returned {resp.status_code}"

            actual = resp.json()[endpoint.get("get_field", endpoint["field"])]

            if endpoint.get("tolerance") is not None:
                assert actual == pytest.approx(value, abs=endpoint["tolerance"]), \
                    f"{endpoint['name']}: expected {value}, got {actual}"
            else:
                # RPM-style: relative tolerance, special case for zero
                if value == 0:
                    assert actual == pytest.approx(0, abs=endpoint.get("rpm_zero_abs", 1))
                else:
                    assert actual == pytest.approx(value, rel=endpoint["rel"]), \
                        f"{endpoint['name']}: expected {value}, got {actual}"

            if "cleanup_url" in endpoint:
                try:
                    requests.get(f"{BASE_URL}{endpoint['cleanup_url']}", timeout=REQUEST_TIMEOUT)
                except Exception:
                    pass


# ==========================================
# Pumps Module
# ==========================================

class TestPumpsSetGet:

    @pytest.mark.parametrize("speed", PUMPS_SPEED_VALUES)
    def test_speed_set_get(self, pump_instances, speed):
        if not pump_instances:
            pytest.skip("No pump instances available")
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                resp = requests.post(
                    f"{BASE_URL}/pumps/{inst}/speed/{pump}",
                    json={"speed": speed},
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200, \
                    f"POST speed={speed} failed for pump {inst}/{pump}"

                resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/speed/{pump}",
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200
                assert resp.json()["speed"] == pytest.approx(speed, abs=0.01), \
                    f"speed mismatch for pump {inst}/{pump}: expected {speed}"

                requests.get(f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)

    def test_flowrate_set_get(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                info = requests.get(
                    f"{BASE_URL}/pumps/{inst}/info/{pump}", timeout=REQUEST_TIMEOUT
                )
                assert info.status_code == 200, f"GET info failed for pump {inst}/{pump}"
                flowrate = float(info.json()["max_flowrate"])

                resp = requests.post(
                    f"{BASE_URL}/pumps/{inst}/flowrate/{pump}",
                    json={"flowrate": flowrate},
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200, \
                    f"POST flowrate={flowrate} failed for pump {inst}/{pump}"

                resp = requests.get(
                    f"{BASE_URL}/pumps/{inst}/flowrate/{pump}",
                    timeout=REQUEST_TIMEOUT,
                )
                assert resp.status_code == 200
                assert resp.json()["flowrate"] == pytest.approx(flowrate, abs=0.1), \
                    f"flowrate mismatch for pump {inst}/{pump}: expected {flowrate}"

                requests.get(f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)
