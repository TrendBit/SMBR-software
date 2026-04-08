import time
import pytest
import requests
from concurrent.futures import ThreadPoolExecutor, as_completed
from ..config import BASE_URL, REQUEST_TIMEOUT
from .concurrency_config import (
    LAST_WRITE_WINS,
    IDEMPOTENT_WRITES,
    SETTLE_AFTER_WRITE_S,
    DELAY_BETWEEN_WRITES_S,
    PUMPS_SPEED_VALUES,
    PUMPS_FLOWRATE_VALUES,
    PUMPS_SPEED_IDEMPOTENT,
    PUMPS_FLOWRATE_IDEMPOTENT,
)


def _post(url: str, field: str, value) -> int:
    resp = requests.post(
        f"{BASE_URL}{url}",
        json={field: value},
        timeout=REQUEST_TIMEOUT,
    )
    return resp.status_code


def _get_value(url: str):
    resp = requests.get(f"{BASE_URL}{url}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, f"GET {url} returned {resp.status_code}"
    data = resp.json()
    for key in ("temperature", "intensity", "speed", "flowrate"):
        if key in data:
            return float(data[key])
    raise ValueError(f"No known field in GET response: {data}")


def _cleanup(cfg: dict):
    try:
        c = cfg.get("cleanup")
        if not c:
            return
        if c["type"] == "get":
            requests.get(f"{BASE_URL}{c['url']}", timeout=REQUEST_TIMEOUT)
        else:
            requests.post(f"{BASE_URL}{c['url']}", json=c["body"], timeout=REQUEST_TIMEOUT)
    except Exception:
        pass


# ==========================================
# Concurrency 
# ==========================================

class TestLastWriteWins:
    """Two simultaneous writes with different values — last write must win."""

    @pytest.mark.parametrize("ep", LAST_WRITE_WINS, ids=lambda e: e["name"])
    def test_last_write_wins(self, ep):
        try:
            with ThreadPoolExecutor(max_workers=2) as executor:
                future_a = executor.submit(_post, ep["post_url"], ep["field"], ep["value_a"])
                time.sleep(DELAY_BETWEEN_WRITES_S)
                future_b = executor.submit(_post, ep["post_url"], ep["field"], ep["value_b"])
                futures = {future_a: ep["value_a"], future_b: ep["value_b"]}
                last_value = None
                for future in as_completed(futures):
                    status = future.result()
                    assert status == 200, (
                        f"{ep['name']}: concurrent POST returned {status}"
                    )
                    last_value = futures[future]

            # Use endpoint-specific settle_s if present, otherwise global
            settle = ep.get("settle_s", SETTLE_AFTER_WRITE_S)
            time.sleep(settle)

            actual = _get_value(ep["get_url"])
            assert actual == pytest.approx(last_value, abs=ep["tolerance"]), (
                f"{ep['name']}: expected GET to return last written value "
                f"{last_value}, got {actual}"
            )
        finally:
            _cleanup(ep)


class TestIdempotentWrite:
    """Two simultaneous writes with the same value — state must remain consistent."""

    @pytest.mark.parametrize("ep", IDEMPOTENT_WRITES, ids=lambda e: e["name"])
    def test_idempotent_write(self, ep):
        try:
            with ThreadPoolExecutor(max_workers=2) as executor:
                future1 = executor.submit(_post, ep["post_url"], ep["field"], ep["value"])
                time.sleep(DELAY_BETWEEN_WRITES_S)
                future2 = executor.submit(_post, ep["post_url"], ep["field"], ep["value"])
                futures = [future1, future2]
                for future in futures:
                    status = future.result()
                    assert status == 200, (
                        f"{ep['name']}: concurrent POST returned {status}"
                    )

            settle = ep.get("settle_s", SETTLE_AFTER_WRITE_S)
            time.sleep(settle)

            actual = _get_value(ep["get_url"])
            assert actual == pytest.approx(ep["value"], abs=ep["tolerance"]), (
                f"{ep['name']}: expected GET to return {ep['value']} after "
                f"idempotent writes, got {actual}"
            )
        finally:
            _cleanup(ep)


# ==========================================
# Pumps Module
# ==========================================

class TestPumpsLastWriteWins:
    """Two simultaneous writes with different values to pumps module — last write must win."""

    def _run_last_write_wins(self, url: str, field: str, value_a, value_b, tolerance: float):
        with ThreadPoolExecutor(max_workers=2) as executor:
            future_a = executor.submit(_post, url, field, value_a)
            time.sleep(DELAY_BETWEEN_WRITES_S)
            future_b = executor.submit(_post, url, field, value_b)
            futures = {future_a: value_a, future_b: value_b}
            last_value = None
            for future in as_completed(futures):
                status = future.result()
                assert status == 200, f"concurrent POST to {url} returned {status}"
                last_value = futures[future]

        time.sleep(SETTLE_AFTER_WRITE_S)

        resp = requests.get(f"{BASE_URL}{url}", timeout=REQUEST_TIMEOUT)
        assert resp.status_code == 200
        actual = float(resp.json()[field])
        assert actual == pytest.approx(last_value, abs=tolerance), (
            f"{url}: expected last written value {last_value}, got {actual}"
        )

    def test_speed_last_write_wins(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")
        value_a, value_b = PUMPS_SPEED_VALUES
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                url = f"/pumps/{inst}/speed/{pump}"
                try:
                    self._run_last_write_wins(url, "speed", value_a, value_b, 0.01)
                finally:
                    requests.get(f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)

    def test_flowrate_last_write_wins(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")
        value_a, value_b = PUMPS_FLOWRATE_VALUES
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                url = f"/pumps/{inst}/flowrate/{pump}"
                try:
                    self._run_last_write_wins(url, "flowrate", value_a, value_b, 0.1)
                finally:
                    requests.get(f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)


class TestPumpsIdempotentWrite:
    """Two simultaneous writes with the same value to pumps module — state must remain consistent."""

    def _run_idempotent(self, url: str, field: str, value, tolerance: float):
        with ThreadPoolExecutor(max_workers=2) as executor:
            future1 = executor.submit(_post, url, field, value)
            time.sleep(DELAY_BETWEEN_WRITES_S)
            future2 = executor.submit(_post, url, field, value)
            for future in [future1, future2]:
                status = future.result()
                assert status == 200, f"concurrent POST to {url} returned {status}"

        time.sleep(SETTLE_AFTER_WRITE_S)

        resp = requests.get(f"{BASE_URL}{url}", timeout=REQUEST_TIMEOUT)
        assert resp.status_code == 200
        actual = float(resp.json()[field])
        assert actual == pytest.approx(value, abs=tolerance), (
            f"{url}: expected {value} after idempotent writes, got {actual}"
        )

    def test_speed_idempotent(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                url = f"/pumps/{inst}/speed/{pump}"
                try:
                    self._run_idempotent(url, "speed", PUMPS_SPEED_IDEMPOTENT, 0.01)
                finally:
                    requests.get(f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)

    def test_flowrate_idempotent(self, pump_instances):
        if not pump_instances:
            pytest.skip("No pump instances available")
        for inst, cnt in pump_instances.items():
            for pump in range(1, cnt + 1):
                url = f"/pumps/{inst}/flowrate/{pump}"
                try:
                    self._run_idempotent(url, "flowrate", PUMPS_FLOWRATE_IDEMPOTENT, 0.1)
                finally:
                    requests.get(f"{BASE_URL}/pumps/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)
