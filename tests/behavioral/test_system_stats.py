import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .system_stats_config import RX_PACKETS_URL, TX_PACKETS_URL, TRIGGER_URL


class TestCanPacketsIncreasing:
    """CAN rx/tx packet counters must be > 0 and must increase after API requests."""

    def _get_packets(self):
        rx_resp = requests.get(f"{BASE_URL}{RX_PACKETS_URL}", timeout=REQUEST_TIMEOUT)
        tx_resp = requests.get(f"{BASE_URL}{TX_PACKETS_URL}", timeout=REQUEST_TIMEOUT)
        assert rx_resp.status_code == 200, (
            f"GET {RX_PACKETS_URL} returned {rx_resp.status_code}"
        )
        assert tx_resp.status_code == 200, (
            f"GET {TX_PACKETS_URL} returned {tx_resp.status_code}"
        )
        return rx_resp.json()["rx_packets"], tx_resp.json()["tx_packets"]

    def test_packets_nonzero(self):
        rx, tx = self._get_packets()
        assert rx > 0, f"rx_packets expected > 0, got {rx}"
        assert tx > 0, f"tx_packets expected > 0, got {tx}"

    def test_packets_increase_after_request(self):
        rx_before, tx_before = self._get_packets()

        requests.get(f"{BASE_URL}{TRIGGER_URL}", timeout=REQUEST_TIMEOUT)

        rx_after, tx_after = self._get_packets()

        assert rx_after > rx_before, (
            f"rx_packets did not increase after GET {TRIGGER_URL}: before={rx_before}, after={rx_after}"
        )
        assert tx_after > tx_before, (
            f"tx_packets did not increase after GET {TRIGGER_URL}: before={tx_before}, after={tx_after}"
        )
