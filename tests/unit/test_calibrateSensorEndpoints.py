"""
Unit tests for calibration endpoints that require exactly empty JSON body {}.
"""

import pytest
import requests

from ..config import BASE_URL, REQUEST_TIMEOUT


CALIBRATE_ENDPOINTS = [
    f"{BASE_URL}/sensor/fluorometer/calibrate",
    f"{BASE_URL}/sensor/spectrophotometer/calibrate",
]


class TestCalibrateSensorEndpoints:
    """Tests for POST calibration endpoints requiring {} body"""

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    def test_valid_empty_json_body(self, endpoint):
        """Valid: endpoint accepts empty JSON object {}"""
        response = requests.post(endpoint, json={}, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 200

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    def test_error_missing_body(self, endpoint):
        """Error: missing request body"""
        response = requests.post(
            endpoint,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
        assert "empty json object" in response.json()["message"].lower()

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    @pytest.mark.parametrize(
        "payload",
        [
            {"calibrationMode": "default"},
            {"extra": 1},
        ],
    )
    def test_error_non_empty_json_body(self, endpoint, payload):
        """Error: non-empty JSON object is rejected"""
        response = requests.post(endpoint, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "empty json object" in response.json()["message"].lower()

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    @pytest.mark.parametrize("invalid_json", ["[]", '"text"', "null"])
    def test_error_invalid_json_structure(self, endpoint, invalid_json):
        """Error: body must be object {}, not array/string/null"""
        response = requests.post(
            endpoint,
            data=invalid_json,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
        assert "empty json object" in response.json()["message"].lower()

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    @pytest.mark.parametrize(
        "invalid_json",
        [
            "{",
            '{"calibrationMode": "default"',
            "{calibrationMode: default}",
        ],
    )
    def test_error_malformed_json(self, endpoint, invalid_json):
        """Error: malformed JSON payload is rejected"""
        response = requests.post(
            endpoint,
            data=invalid_json,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    def test_content_type_text_plain_with_empty_object(self, endpoint):
        """Framework behavior: text/plain with {} is still accepted"""
        response = requests.post(
            endpoint,
            data="{}",
            headers={"Content-Type": "text/plain"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    def test_error_whitespace_only_empty_object(self, endpoint):
        """Error: body must be exactly {}, whitespace variant is rejected"""
        response = requests.post(
            endpoint,
            data="{ }",
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
        assert "empty json object" in response.json()["message"].lower()

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    def test_error_duplicate_keys(self, endpoint):
        """Error: duplicate keys payload is not accepted (not exactly {})"""
        response = requests.post(
            endpoint,
            data='{"a": 1, "a": 2}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
        assert "empty json object" in response.json()["message"].lower()

    @pytest.mark.parametrize("endpoint", CALIBRATE_ENDPOINTS)
    def test_error_trailing_comma(self, endpoint):
        """Error: trailing comma object is rejected"""
        response = requests.post(
            endpoint,
            data='{"calibrationMode": null,}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
