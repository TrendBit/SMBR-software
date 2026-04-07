import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .thermal_effects_config import (
    LED_PANEL_INTENSITY_URL,
    LED_PANEL_TEMP_URL,
    LED_PANEL_ZERO_POST,
    LED_FULL_INTENSITY,
    LED_HEAT_WAIT_S,
    LED_RISE_MIN_DELTA_C,
    LED_COOL_WAIT_S,
    LED_COOL_MIN_DELTA_C,
    FLUOROMETER_SINGLE_URL,
    FLUOROMETER_EMITOR_URL,
    FLUORO_SAMPLE_COUNT,
    FLUORO_DETECTOR_GAIN,
    FLUORO_EMITOR_INTENSITY,
    FLUORO_EMITOR_RISE_MIN_DELTA_C,
    FLUORO_EMITOR_COOL_WAIT_S,
    FLUORO_EMITOR_COOL_MIN_DELTA_C,
    SPECTRO_MEASURE_URL,
    SPECTRO_EMITOR_URL,
    SPECTRO_SAMPLE_COUNT,
    SPECTRO_EMITOR_RISE_MIN_DELTA_C,
    SPECTRO_EMITOR_COOL_WAIT_S,
    SPECTRO_EMITOR_COOL_MIN_DELTA_C,
)


def _get_led_temperature() -> float:
    resp = requests.get(f"{BASE_URL}{LED_PANEL_TEMP_URL}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, f"GET {LED_PANEL_TEMP_URL} returned {resp.status_code}"
    return float(resp.json()["temperature"])


def _get_fluorometer_emitor_temperature() -> float:
    resp = requests.get(f"{BASE_URL}{FLUOROMETER_EMITOR_URL}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, f"GET {FLUOROMETER_EMITOR_URL} returned {resp.status_code}"
    return float(resp.json()["temperature"])



def _get_spectrophotometer_emitor_temperature() -> float:
    resp = requests.get(f"{BASE_URL}{SPECTRO_EMITOR_URL}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, f"GET {SPECTRO_EMITOR_URL} returned {resp.status_code}"
    return float(resp.json()["temperature"])


def _fire_spectrophotometer_samples(count: int):
    for i in range(count):
        resp = requests.post(
            f"{BASE_URL}{SPECTRO_MEASURE_URL}",
            timeout=REQUEST_TIMEOUT,
        )
        assert resp.status_code == 200, (
            f"Spectrophotometer measure_all #{i + 1} returned {resp.status_code}"
        )


def _set_led_intensity(intensity: list):
    resp = requests.post(
        f"{BASE_URL}{LED_PANEL_INTENSITY_URL}",
        json={"intensity": intensity},
        timeout=REQUEST_TIMEOUT,
    )
    assert resp.status_code == 200, (
        f"POST {LED_PANEL_INTENSITY_URL} intensity={intensity} returned {resp.status_code}"
    )


def _zero_led():
    try:
        cp = LED_PANEL_ZERO_POST
        requests.post(
            f"{BASE_URL}{cp['post_url']}",
            json={cp["field"]: cp["value"]},
            timeout=REQUEST_TIMEOUT,
        )
    except Exception:
        pass


def _fire_fluorometer_samples(count: int):
    for i in range(count):
        resp = requests.post(
            f"{BASE_URL}{FLUOROMETER_SINGLE_URL}",
            json={"detector_gain": FLUORO_DETECTOR_GAIN, "emitor_intensity": FLUORO_EMITOR_INTENSITY},
            timeout=REQUEST_TIMEOUT,
        )
        assert resp.status_code == 200, (
            f"Single sample #{i + 1} returned {resp.status_code}"
        )


# ==========================================
# Thermal effects of components
# ==========================================

class TestLedPanelThermalEffects:
    """Full-intensity LED panel operation must raise the LED temperature sensor."""

    @pytest.fixture(autouse=True)
    def zero_led_after(self):
        yield
        _zero_led()

    def test_led_temperature_increases_under_load(self):
        """LED temperature must rise after running at full intensity."""
        temp_before = _get_led_temperature()
        
        _set_led_intensity(LED_FULL_INTENSITY)
        
        time.sleep(LED_HEAT_WAIT_S)

        temp_after = _get_led_temperature()
        delta = temp_after - temp_before

        assert delta >= LED_RISE_MIN_DELTA_C, (
            f"LED temperature did not rise enough: before={temp_before:.2f} °C, "
            f"after={temp_after:.2f} °C, rise={delta:+.2f} °C "
            f"(min expected={LED_RISE_MIN_DELTA_C} °C)"
        )

    def test_led_temperature_decreases_after_idle(self):
        """LED temperature must drop after the panel is turned off."""
        _set_led_intensity(LED_FULL_INTENSITY)
        
        time.sleep(LED_HEAT_WAIT_S)

        temp_hot = _get_led_temperature()

        _zero_led()
        
        time.sleep(LED_COOL_WAIT_S)

        temp_cooled = _get_led_temperature()
        delta = temp_hot - temp_cooled
    
        assert delta >= LED_COOL_MIN_DELTA_C, (
            f"LED temperature did not drop after idle: "
            f"hot={temp_hot:.2f} °C, cooled={temp_cooled:.2f} °C, "
            f"drop={delta:+.2f} °C (min expected={LED_COOL_MIN_DELTA_C} °C)"
        )


class TestFluorometerEmitorThermalEffects:
    """Rapid fluorometer sampling must raise the emitor temperature."""

    def test_emitor_temperature_increases_under_load(self):
        """Emitor temperature must rise after a burst of rapid single-sample calls."""
        temp_before = _get_fluorometer_emitor_temperature()
        
        _fire_fluorometer_samples(FLUORO_SAMPLE_COUNT)

        temp_after = _get_fluorometer_emitor_temperature()
        delta = temp_after - temp_before
    
        assert delta >= FLUORO_EMITOR_RISE_MIN_DELTA_C, (
            f"Emitor temperature did not rise enough: before={temp_before:.2f} °C, "
            f"after={temp_after:.2f} °C, rise={delta:+.2f} °C "
            f"(min expected={FLUORO_EMITOR_RISE_MIN_DELTA_C} °C)"
        )

    def test_emitor_temperature_decreases_after_idle(self):
        """Emitor temperature must drop after sampling stops and system is idle."""
        
        _fire_fluorometer_samples(FLUORO_SAMPLE_COUNT)

        temp_hot = _get_fluorometer_emitor_temperature()
        
        time.sleep(FLUORO_EMITOR_COOL_WAIT_S)

        temp_cooled = _get_fluorometer_emitor_temperature()
        delta = temp_hot - temp_cooled
        
        assert delta >= FLUORO_EMITOR_COOL_MIN_DELTA_C, (
            f"Emitor temperature did not drop after idle: "
            f"hot={temp_hot:.2f} °C, cooled={temp_cooled:.2f} °C, "
            f"drop={delta:+.2f} °C (min expected={FLUORO_EMITOR_COOL_MIN_DELTA_C} °C)"
        )


class TestSpectrophotometerEmitorThermalEffects:
    """Rapid spectrophotometer measurement must raise the emitor temperature."""

    def test_emitor_temperature_increases_under_load(self):
        """Spectrophotometer emitor temperature must rise after a burst of rapid measure_all calls."""
        temp_before = _get_spectrophotometer_emitor_temperature()
        
        _fire_spectrophotometer_samples(SPECTRO_SAMPLE_COUNT)

        temp_after = _get_spectrophotometer_emitor_temperature()
        delta = temp_after - temp_before 

        assert delta >= SPECTRO_EMITOR_RISE_MIN_DELTA_C, (
            f"Spectrophotometer emitor temperature did not rise enough: before={temp_before:.2f} °C, "
            f"after={temp_after:.2f} °C, rise={delta:+.2f} °C "
            f"(min expected={SPECTRO_EMITOR_RISE_MIN_DELTA_C} °C)"
        )

    def test_emitor_temperature_decreases_after_idle(self):
        """Spectrophotometer emitor temperature must drop after measurement stops and system is idle."""
        
        _fire_spectrophotometer_samples(SPECTRO_SAMPLE_COUNT)

        temp_hot = _get_spectrophotometer_emitor_temperature()
        
        time.sleep(SPECTRO_EMITOR_COOL_WAIT_S)

        temp_cooled = _get_spectrophotometer_emitor_temperature()
        delta = temp_hot - temp_cooled

        assert delta >= SPECTRO_EMITOR_COOL_MIN_DELTA_C, (
            f"Spectrophotometer emitor temperature did not drop after idle: "
            f"hot={temp_hot:.2f} °C, cooled={temp_cooled:.2f} °C, "
            f"drop={delta:+.2f} °C (min expected={SPECTRO_EMITOR_COOL_MIN_DELTA_C} °C)"
        )
