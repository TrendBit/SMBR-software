"""
Unit tests for POST /sensor/spectrophotometer/measure/{channel} endpoint.
"""

import pytest
import requests

from ..config import BASE_URL, REQUEST_TIMEOUT


BASE_ENDPOINT = f"{BASE_URL}/sensor/spectrophotometer/measure"


class TestMeasureSingleSpectrophotometerChannel:
    """Tests for POST /sensor/spectrophotometer/measure/{channel}"""

    @pytest.mark.parametrize("channel", range(0, 6))
    def test_valid_channels_0_to_5(self, channel):
        """Valid: channels in range 0-5 should be accepted"""
        response = requests.post(
            f"{BASE_ENDPOINT}/{channel}",
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    def test_valid_response_contains_expected_fields(self):
        """Valid: 200 response contains expected measurement fields"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

        data = response.json()
        assert "channel" in data
        assert "relative_value" in data
        assert "absolute_value" in data
        assert isinstance(data["channel"], int)
        assert isinstance(data["relative_value"], (int, float))
        assert isinstance(data["absolute_value"], int)

    @pytest.mark.parametrize("channel", [-1, 7])
    def test_invalid_channels_out_of_range(self, channel):
        """Invalid: channels outside 0-5 should fail"""
        response = requests.post(
            f"{BASE_ENDPOINT}/{channel}",
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 504

    @pytest.mark.parametrize("channel", ["abc", "1.5"])
    def test_invalid_channel_path_format(self, channel):
        """Invalid: non-integer channel path parameter"""
        response = requests.post(
            f"{BASE_ENDPOINT}/{channel}",
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 400
