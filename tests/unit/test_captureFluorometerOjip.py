"""
Unit tests for POST /sensor/fluorometer/ojip/capture endpoint.
"""

import pytest
import requests
import time

from ..config import BASE_URL, REQUEST_TIMEOUT


ENDPOINT = f"{BASE_URL}/sensor/fluorometer/ojip/capture"
OJIP_REQUEST_TIMEOUT = max(REQUEST_TIMEOUT, 25)
CAPTURE_RETRY_COUNT = 3
CAPTURE_RETRY_DELAY_SEC = 1.0
CAPTURE_TEST_GAP_SEC = 1.0
CAPTURE_FIRST_TEST_GAP_SEC = 5.0


def valid_payload():
    return {
        "detector_gain": "x1",
        "timebase": "linear",
        "emitor_intensity": 0.5,
        "length_ms": 200,
        "sample_count": 200,
    }


class TestCaptureFluorometerOjip:
    """Tests for POST /sensor/fluorometer/ojip/capture"""

    _first_valid_gap_done = False

    @pytest.fixture(autouse=True)
    def _gap_between_capture_tests(self, request):
        test_name = getattr(request.node, "originalname", request.node.name)
        if test_name.startswith("test_valid_"):
            if not request.cls._first_valid_gap_done:
                time.sleep(CAPTURE_FIRST_TEST_GAP_SEC)
                request.cls._first_valid_gap_done = True
            else:
                time.sleep(CAPTURE_TEST_GAP_SEC)
        yield

    def test_valid_response_shape_when_capture_succeeds(self):
        """Valid: capture request returns 200"""
        response = None
        for attempt in range(CAPTURE_RETRY_COUNT):
            response = requests.post(ENDPOINT, json=valid_payload(), timeout=OJIP_REQUEST_TIMEOUT)
            if response.status_code != 504:
                break
            if attempt < CAPTURE_RETRY_COUNT - 1:
                time.sleep(CAPTURE_RETRY_DELAY_SEC)

        assert response.status_code == 200

    @pytest.mark.parametrize("detector_gain", ["x1", "x10", "x50", "Auto"])
    @pytest.mark.parametrize("timebase", ["linear", "logarithmic"])
    def test_valid_minimum_request_combinations(self, detector_gain, timebase):
        """Valid: accepted detector_gain and timebase combinations with minimum limits"""
        payload = valid_payload()
        payload["detector_gain"] = detector_gain
        payload["timebase"] = timebase

        response = requests.post(ENDPOINT, json=payload, timeout=OJIP_REQUEST_TIMEOUT)
        assert response.status_code == 200

    @pytest.mark.parametrize("length_ms", [200, 4000])
    def test_valid_length_boundaries(self, length_ms):
        """Valid: length_ms boundaries 200 and 4000"""
        payload = valid_payload()
        payload["length_ms"] = length_ms

        response = requests.post(ENDPOINT, json=payload, timeout=OJIP_REQUEST_TIMEOUT)
        assert response.status_code == 200

    @pytest.mark.parametrize("sample_count", [200, 4000])
    def test_valid_sample_count_boundaries(self, sample_count):
        """Valid: sample_count boundaries 200 and 4000"""
        payload = valid_payload()
        payload["sample_count"] = sample_count

        response = requests.post(ENDPOINT, json=payload, timeout=OJIP_REQUEST_TIMEOUT)
        assert response.status_code == 200

    @pytest.mark.parametrize("emitor_intensity", [0.2, 1.0])
    def test_valid_emitor_intensity_boundaries(self, emitor_intensity):
        """Valid: emitor_intensity boundaries 0.2 and 1.0"""
        payload = valid_payload()
        payload["emitor_intensity"] = emitor_intensity

        response = requests.post(ENDPOINT, json=payload, timeout=OJIP_REQUEST_TIMEOUT)
        assert response.status_code == 200

    @pytest.mark.parametrize(
        "payload",
        [
            {},
            {"detector_gain": "x1"},
            {
                "detector_gain": "x1",
                "timebase": "linear",
                "emitor_intensity": 0.5,
                "length_ms": 200,
            },
            {
                "detector_gain": "x1",
                "timebase": "linear",
                "emitor_intensity": 0.5,
                "sample_count": 200,
            },
        ],
    )
    def test_error_missing_required_parameters(self, payload):
        """Error: one or more required parameters are missing"""
        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "missing required parameters" in response.json()["message"].lower()

    @pytest.mark.parametrize("length_ms", [199, 4001, -1])
    def test_error_invalid_length_ms(self, length_ms):
        """Error: length_ms outside 200-4000"""
        payload = valid_payload()
        payload["length_ms"] = length_ms

        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "invalid length" in response.json()["message"].lower()

    @pytest.mark.parametrize("sample_count", [199, 4001, -1])
    def test_error_invalid_sample_count(self, sample_count):
        """Error: sample_count outside 200-4000"""
        payload = valid_payload()
        payload["sample_count"] = sample_count

        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "invalid sample count" in response.json()["message"].lower()

    @pytest.mark.parametrize("emitor_intensity", [0.1999, 1.0001, -1.0, 2.0])
    def test_error_invalid_emitor_intensity(self, emitor_intensity):
        """Error: emitor_intensity outside 0.2-1.0"""
        payload = valid_payload()
        payload["emitor_intensity"] = emitor_intensity

        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "invalid emitor intensity" in response.json()["message"].lower()

    @pytest.mark.parametrize("detector_gain", ["x2", "auto", "", "x100"])
    def test_error_invalid_detector_gain(self, detector_gain):
        """Error: detector_gain must be one of x1, x10, x50, Auto"""
        payload = valid_payload()
        payload["detector_gain"] = detector_gain

        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "invalid detector_gain" in response.json()["message"].lower()

    @pytest.mark.parametrize("timebase", ["log", "lin", "", "LOGARITHMIC"])
    def test_error_invalid_timebase(self, timebase):
        """Error: timebase must be linear or logarithmic"""
        payload = valid_payload()
        payload["timebase"] = timebase

        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400
        assert "invalid timebase" in response.json()["message"].lower()

    @pytest.mark.parametrize(
        "payload",
        [
            {"detector_gain": None, "timebase": "linear", "emitor_intensity": 0.5, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": None, "emitor_intensity": 0.5, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": None, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": 0.5, "length_ms": None, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": 0.5, "length_ms": 200, "sample_count": None},
        ],
    )
    def test_error_null_required_parameters(self, payload):
        """Error: null in required parameters is rejected"""
        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400

    @pytest.mark.parametrize(
        "payload",
        [
            {"detector_gain": 1, "timebase": "linear", "emitor_intensity": 0.5, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": 1, "emitor_intensity": 0.5, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": "0.5", "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": 0.5, "length_ms": "200", "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": 0.5, "length_ms": 200, "sample_count": "200"},
            {"detector_gain": ["x1"], "timebase": "linear", "emitor_intensity": 0.5, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": ["linear"], "emitor_intensity": 0.5, "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": [0.5], "length_ms": 200, "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": 0.5, "length_ms": [200], "sample_count": 200},
            {"detector_gain": "x1", "timebase": "linear", "emitor_intensity": 0.5, "length_ms": 200, "sample_count": [200]},
        ],
    )
    def test_error_wrong_types_in_parameters(self, payload):
        """Error: wrong JSON types for required parameters are rejected"""
        response = requests.post(ENDPOINT, json=payload, timeout=REQUEST_TIMEOUT)
        assert response.status_code == 400

    @pytest.mark.parametrize(
        "invalid_json",
        [
            "{",
            '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200',
            '{detector_gain:"x1",timebase:"linear",emitor_intensity:0.5,length_ms:200,sample_count:200}',
        ],
    )
    def test_error_malformed_json(self, invalid_json):
        """Error: malformed JSON body is rejected"""
        response = requests.post(
            ENDPOINT,
            data=invalid_json,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    def test_error_missing_request_body(self):
        """Error: missing request body"""
        response = requests.post(
            ENDPOINT,
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400

    def test_error_wrong_content_type(self):
        """Error: wrong content type for DTO endpoint"""
        response = requests.post(
            ENDPOINT,
            data='{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}',
            headers={"Content-Type": "text/plain"},
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
