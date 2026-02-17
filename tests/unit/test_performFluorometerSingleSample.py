"""
Unit tests for POST /sensor/fluorometer/single_sample endpoint.
"""

import pytest
import requests

from ..config import BASE_URL, REQUEST_TIMEOUT


ENDPOINT = f"{BASE_URL}/sensor/fluorometer/single_sample"


class TestSingleSampleFluorometer:
    """Tests for POST /sensor/fluorometer/single_sample"""

    @pytest.mark.parametrize("detector_gain", ["x1", "x10", "x50", "Auto"])
    def test_valid_detector_gain_values(self, detector_gain):
        """Valid: detector_gain accepts x1, x10, x50, Auto"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": detector_gain, "emitor_intensity": 0.5},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    def test_valid_response_contains_expected_fields(self):
        """Valid: 200 response contains expected numeric fields"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": "x1", "emitor_intensity": 0.5},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200
        data = response.json()
        assert "raw_value" in data
        assert "relative_value" in data
        assert "absolute_value" in data
        assert isinstance(data["raw_value"], int)
        assert isinstance(data["relative_value"], (int, float))
        assert isinstance(data["absolute_value"], (int, float))

    @pytest.mark.parametrize(
        "detector_gain,emitor_intensity",
        [
            ("x1", 0.2),
            ("x10", 0.5),
            ("x50", 1.0),
            ("Auto", 0.2),
            ("Auto", 1.0),
        ],
    )
    def test_valid_gain_intensity_combinations(self, detector_gain, emitor_intensity):
        """Valid: selected detector_gain and emitor_intensity combinations"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": detector_gain, "emitor_intensity": emitor_intensity},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    @pytest.mark.parametrize("emitor_intensity", [0.2, 1.0])
    def test_valid_emitor_intensity_boundaries(self, emitor_intensity):
        """Valid: emitor_intensity boundaries 0.2 and 1.0"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": "x1", "emitor_intensity": emitor_intensity},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    @pytest.mark.parametrize("emitor_intensity", [0.1999, 1.0001, -1.0, 2.0])
    def test_invalid_emitor_intensity_out_of_range(self, emitor_intensity):
        """Error: emitor_intensity outside 0.2-1.0"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": "x1", "emitor_intensity": emitor_intensity},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize("detector_gain", ["x2", "auto", "AUTO", "", "x100"])
    def test_invalid_detector_gain_values(self, detector_gain):
        """Error: detector_gain must be x1, x10, x50, or Auto"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": detector_gain, "emitor_intensity": 0.5},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize(
        "payload",
        [
            {},
            {"detector_gain": "x1"},
            {"emitor_intensity": 0.5},
            {"detector_gain": None, "emitor_intensity": 0.5},
            {"detector_gain": "x1", "emitor_intensity": None},
        ],
    )
    def test_missing_or_null_required_fields(self, payload):
        """Error: detector_gain and emitor_intensity are required"""
        response = requests.post(
            ENDPOINT,
            json=payload,
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize(
        "invalid_json",
        [
            '{"detector_gain": "x1", "emitor_intensity": 0.5',
            '{detector_gain: "x1", emitor_intensity: 0.5}',
        ],
    )
    def test_invalid_json(self, invalid_json):
        """Error: invalid JSON syntax"""
        response = requests.post(
            ENDPOINT,
            data=invalid_json,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    def test_missing_request_body(self):
        """Error: missing request body with JSON content type"""
        response = requests.post(
            ENDPOINT,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    def test_wrong_content_type_header(self):
        """Error: wrong Content-Type header"""
        response = requests.post(
            ENDPOINT,
            data='{"detector_gain": "x1", "emitor_intensity": 0.5}',
            headers={"Content-Type": "text/plain"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize(
        "detector_gain",
        [
            1,
            True,
            ["x1"],
            {"gain": "x1"},
        ],
    )
    def test_wrong_type_detector_gain(self, detector_gain):
        """Error: wrong type for detector_gain"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": detector_gain, "emitor_intensity": 0.5},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize(
        "emitor_intensity",
        [
            "0.5",
            [0.5],
            {"value": 0.5},
        ],
    )
    def test_wrong_type_emitor_intensity(self, emitor_intensity):
        """Error: wrong type for emitor_intensity"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": "x1", "emitor_intensity": emitor_intensity},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize("special_value", ["Infinity", "-Infinity", "NaN"])
    def test_special_float_values_emitor_intensity(self, special_value):
        """Error: special float values for emitor_intensity"""
        response = requests.post(
            ENDPOINT,
            data=f'{{"detector_gain": "x1", "emitor_intensity": {special_value}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    def test_additional_property_simple(self):
        """Framework limitation: additional simple property is accepted"""
        response = requests.post(
            ENDPOINT,
            json={"detector_gain": "x1", "emitor_intensity": 0.5, "extra": "value"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    def test_additional_property_complex(self):
        """Framework limitation: additional complex property is accepted"""
        response = requests.post(
            ENDPOINT,
            json={
                "detector_gain": "x1",
                "emitor_intensity": 0.5,
                "extra": {"nested": True},
            },
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    def test_duplicate_keys_last_value_used(self):
        """Framework limitation: duplicate keys are accepted"""
        response = requests.post(
            ENDPOINT,
            data='{"detector_gain": "x1", "emitor_intensity": 0.4, "emitor_intensity": 0.5}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    def test_trailing_comma_accepted(self):
        """Framework limitation: trailing comma is accepted by parser"""
        response = requests.post(
            ENDPOINT,
            data='{"detector_gain": "x1", "emitor_intensity": 0.5,}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200
