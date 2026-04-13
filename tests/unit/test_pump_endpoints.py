"""
Tests for the pump-related endpoints added in recent refactor.
"""
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT


BASE_PUMPS = f"{BASE_URL}/pumps"


def discover_instances():
    """Return mapping of instance_index -> pump_count for reachable pump modules."""
    out = {}
    for idx in range(1, 13):
        r = requests.get(f"{BASE_PUMPS}/{idx}/pump_count", timeout=REQUEST_TIMEOUT)
        if r.status_code == 200:
            try:
                cnt = int(r.json().get("pump_count", 0))
            except Exception:
                cnt = 0
            out[idx] = cnt
    return out

@pytest.fixture(scope="module")
def pump_info():
    """Pytest fixture yielding discovered instances mapping."""
    info = discover_instances()
    assert info, "no pump instances available"
    return info

@pytest.mark.parametrize("instance", [0, 13])
def test_get_pump_count_instance_out_of_range(instance):
    r = requests.get(f"{BASE_PUMPS}/{instance}/pump_count", timeout=REQUEST_TIMEOUT)
    assert r.status_code == 400

def test_per_instance_pump_info(pump_info):
    """For each available instance check pump_count and getPumpInfo plus invalid indexes."""
    info = pump_info
    for inst, cnt in info.items():
        rcount = requests.get(f"{BASE_PUMPS}/{inst}/pump_count", timeout=REQUEST_TIMEOUT)
        assert rcount.status_code == 200
        assert int(rcount.json().get("pump_count", -1)) == cnt

        for pump in range(1, cnt + 1):
            r = requests.get(f"{BASE_PUMPS}/{inst}/info/{pump}", timeout=REQUEST_TIMEOUT)
            assert r.status_code == 200, f"{inst}/{pump} info failed (status {r.status_code})"

        for bad in (0, cnt + 1):
            rbad = requests.get(f"{BASE_PUMPS}/{inst}/info/{bad}", timeout=REQUEST_TIMEOUT)
            assert rbad.status_code == 400

def iterate_pumps(pump_info):
    for inst, cnt in pump_info.items():
        for pump in range(1, cnt + 1):
            yield inst, pump

def test_speed_get_valid(pump_info):
    """GET speed should succeed for every available pump."""
    for inst, pump in iterate_pumps(pump_info):
        r = requests.get(f"{BASE_PUMPS}/{inst}/speed/{pump}", timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200

def test_speed_post_valid(pump_info):
    """POST speed zero must return 200."""
    for inst, pump in iterate_pumps(pump_info):
        r = requests.post(f"{BASE_PUMPS}/{inst}/speed/{pump}", json={"speed": 0.0}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200

def test_speed_invalid_indexes(pump_info):
    """Out-of-range instance or pump indices should be rejected."""
    for inst, cnt in pump_info.items():
        for bad in (0, cnt + 1):
            r = requests.get(f"{BASE_PUMPS}/{inst}/speed/{bad}", timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
    for bad_inst in (0, 13):
        r = requests.get(f"{BASE_PUMPS}/{bad_inst}/speed/1", timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

def test_speed_invalid_bodies(pump_info):
    """POST payloads with missing/invalid data should fail."""
    min_val, max_val = -1.0, 1.0
    for inst, pump in iterate_pumps(pump_info):
        url = f"{BASE_PUMPS}/{inst}/speed/{pump}"
        r = requests.post(url, json={}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"speed": None}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        for bad in (min_val - 0.001, max_val + 0.001, min_val - 100.0, max_val + 100.0,
                    1e10, -6.68e16):
            r = requests.post(url, json={"speed": bad}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, json={"speed": False}, timeout=REQUEST_TIMEOUT)
        if min_val <= 0.0 <= max_val:
            assert r.status_code == 200
        else:
            assert r.status_code == 400
        r = requests.post(url, json={"speed": True}, timeout=REQUEST_TIMEOUT)
        if min_val <= 1.0 <= max_val:
            assert r.status_code == 200
        else:
            assert r.status_code == 400
        for bad in ["0.5", [0.5], {"speed": "fast"}]:
            r = requests.post(url, json={"speed": bad}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, data='{"speed": 0.5', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{speed:0.5}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{"speed": Infinity}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{"speed": NaN}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

def test_flowrate_get_valid(pump_info):
    """GET flowrate should return 200 for each known pump."""
    for inst, pump in iterate_pumps(pump_info):
        r = requests.get(f"{BASE_PUMPS}/{inst}/flowrate/{pump}", timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200

def test_flowrate_post_valid(pump_info):
    """POST flowrate zero must succeed."""
    for inst, pump in iterate_pumps(pump_info):
        r = requests.post(f"{BASE_PUMPS}/{inst}/flowrate/{pump}", json={"flowrate": 0.0}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200

def test_flowrate_invalid_indexes(pump_info):
    """Flowrate endpoints reject nonexistent pumps or instances."""
    for inst, cnt in pump_info.items():
        for bad in (0, cnt + 1):
            r = requests.get(f"{BASE_PUMPS}/{inst}/flowrate/{bad}", timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
    for bad_inst in (0, 13):
        r = requests.get(f"{BASE_PUMPS}/{bad_inst}/flowrate/1", timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

def test_flowrate_invalid_bodies(pump_info):
    """POST flowrate with malformed or wrong data must 400."""
    min_val, max_val = -1000.0, 1000.0
    for inst, pump in iterate_pumps(pump_info):
        url = f"{BASE_PUMPS}/{inst}/flowrate/{pump}"
        r = requests.post(url, json={}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"flowrate": None}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        for bad in (min_val - 0.001, max_val + 0.001, min_val - 100.0, max_val + 100.0,
                    1e10, -6.68e16):
            r = requests.post(url, json={"flowrate": bad}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, json={"flowrate": False}, timeout=REQUEST_TIMEOUT)
        if min_val <= 0.0 <= max_val:
            assert r.status_code == 200
        else:
            assert r.status_code == 400
        r = requests.post(url, json={"flowrate": True}, timeout=REQUEST_TIMEOUT)
        if min_val <= 1.0 <= max_val:
            assert r.status_code == 200
        else:
            assert r.status_code == 400
        for bad in ["0.5", [0.5], {"flowrate": "fast"}]:
            r = requests.post(url, json={"flowrate": bad}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, data='{"flowrate": 0.5', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{flowrate:0.5}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{"flowrate": Infinity}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{"flowrate": NaN}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

def test_calibration_valid(pump_info):
    """Calibration POST with zero flowrate should succeed for every pump."""
    for inst, pump in iterate_pumps(pump_info):
        info = requests.get(f"{BASE_PUMPS}/{inst}/info/{pump}", timeout=REQUEST_TIMEOUT)
        original_flowrate = float(info.json()["max_flowrate"]) if info.status_code == 200 else 0.0

        r = requests.post(f"{BASE_PUMPS}/{inst}/calibration/{pump}", json={"flowrate": 0.0}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200
        requests.post(f"{BASE_PUMPS}/{inst}/calibration/{pump}", json={"flowrate": original_flowrate}, timeout=REQUEST_TIMEOUT)

def test_calibration_invalid(pump_info):
    """Calibration endpoint should reject bad indexes and payloads."""
    info = pump_info
    for inst, cnt in info.items():
        for bad in (0, cnt + 1):
            r = requests.post(f"{BASE_PUMPS}/{inst}/calibration/{bad}", json={"flowrate": 0.0}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
    for bad_inst in (0, 13):
        r = requests.post(f"{BASE_PUMPS}/{bad_inst}/calibration/1", json={"flowrate": 0.0}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

    min_val, max_val = -1000.0, 1000.0
    for inst, pump in iterate_pumps(pump_info):
        url = f"{BASE_PUMPS}/{inst}/calibration/{pump}"

        orig_info = requests.get(f"{BASE_PUMPS}/{inst}/info/{pump}", timeout=REQUEST_TIMEOUT)
        original_flowrate = float(orig_info.json()["max_flowrate"]) if orig_info.status_code == 200 else 0.0
        r = requests.post(url, json={}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"flowrate": None}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        for bad in (min_val - 0.001, max_val + 0.001, min_val - 100.0, max_val + 100.0,
                    1e10, -6.68e16):
            r = requests.post(url, json={"flowrate": bad}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, json={"flowrate": False}, timeout=REQUEST_TIMEOUT)
        if min_val <= 0.0 <= max_val:
            assert r.status_code == 200
        else:
            assert r.status_code == 400
        r = requests.post(url, json={"flowrate": True}, timeout=REQUEST_TIMEOUT)
        if min_val <= 1.0 <= max_val:
            assert r.status_code == 200
        else:
            assert r.status_code == 400
        for bad in ["0.5", [0.5], {"flowrate": "fast"}]:
            r = requests.post(url, json={"flowrate": bad}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, data='{"flowrate": 0.5', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{flowrate:0.5}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{"flowrate": Infinity}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{"flowrate": NaN}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        requests.post(url, json={"flowrate": original_flowrate}, timeout=REQUEST_TIMEOUT)

def test_move_valid(pump_info):
    """Move command with zero volume/flowrate must work on every pump."""
    for inst, pump in iterate_pumps(pump_info):
        r = requests.post(
            f"{BASE_PUMPS}/{inst}/move/{pump}",
            json={"volume": 0.0, "flowrate": 0.0},
            timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200

def test_move_invalid_indexes(pump_info):
    """Move endpoint rejects nonexistent pumps or instances."""
    info = pump_info
    for inst, cnt in info.items():
        for bad in (0, cnt + 1):
            r = requests.post(
                f"{BASE_PUMPS}/{inst}/move/{bad}",
                json={"volume": 0.0, "flowrate": 0.0},
                timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
    for bad_inst in (0, 13):
        r = requests.post(
            f"{BASE_PUMPS}/{bad_inst}/move/1",
            json={"volume": 0.0, "flowrate": 0.0},
            timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

def test_move_invalid_body(pump_info):
    """Sending incomplete or bad JSON to move should fail."""
    vol_min, vol_max = -1000.0, 1000.0
    fr_min, fr_max = 0.0, 1000.0
    for inst, pump in iterate_pumps(pump_info):
        url = f"{BASE_PUMPS}/{inst}/move/{pump}"
        r = requests.post(url, headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"volume": (vol_min+vol_max)/2}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"flowrate": (fr_min+fr_max)/2}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"volume": None, "flowrate": (fr_min+fr_max)/2}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"volume": (vol_min+vol_max)/2, "flowrate": None}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"volume": None, "flowrate": None}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"volume": "1", "flowrate": 1.0}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, json={"volume": 1.0, "flowrate": "1"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        for vol in (vol_min - 0.01, vol_max + 0.01, -1001.0, 1001.0):
            r = requests.post(url, json={"volume": vol, "flowrate": 0.0}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        for fr in (fr_min - 0.01, fr_max + 0.01, -1.0, 1001.0):
            r = requests.post(url, json={"volume": 0.0, "flowrate": fr}, timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
        r = requests.post(url, data='{"volume": 1.0, "flowrate": 1.0', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
        r = requests.post(url, data='{volume:1,flowrate:1}', headers={"Content-Type": "application/json"}, timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400

def test_stop_get_valid(pump_info):
    """GET stop should work for every pump."""
    for inst, pump in iterate_pumps(pump_info):
        r = requests.get(f"{BASE_PUMPS}/{inst}/stop/{pump}", timeout=REQUEST_TIMEOUT)
        assert r.status_code == 200

def test_stop_invalid_indexes(pump_info):
    """Stop endpoint rejects invalid indices."""
    info = pump_info
    for inst, cnt in info.items():
        for bad in (0, cnt + 1):
            r = requests.get(f"{BASE_PUMPS}/{inst}/stop/{bad}", timeout=REQUEST_TIMEOUT)
            assert r.status_code == 400
    for bad_inst in (0, 13):
        r = requests.get(f"{BASE_PUMPS}/{bad_inst}/stop/1", timeout=REQUEST_TIMEOUT)
        assert r.status_code == 400
