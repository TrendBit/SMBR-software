"""
Unit tests for GET /sensor/spectrophotometer/channel_info/{channel} endpoint.
"""

import pytest
import requests

from ..config import BASE_URL, REQUEST_TIMEOUT


ENDPOINT_TEMPLATE = f"{BASE_URL}/sensor/spectrophotometer/channel_info/{{channel}}"


class TestSpectrophotometerChannelInfo:
    """Tests for GET /sensor/spectrophotometer/channel_info/{channel}"""

    @pytest.mark.parametrize("channel", range(0, 6))
    def test_valid_channels_0_to_5(self, channel):
        """Valid: channels in range 0-5 should be accepted"""
        response = requests.get(
            ENDPOINT_TEMPLATE.format(channel=channel),
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 200

    @pytest.mark.parametrize("channel", [-1, 7])
    def test_invalid_channels_out_of_range(self, channel):
        """Invalid: channels outside 0-5 should be rejected"""
        response = requests.get(
            ENDPOINT_TEMPLATE.format(channel=channel),
            timeout=REQUEST_TIMEOUT,
        )
        assert response.status_code == 504
