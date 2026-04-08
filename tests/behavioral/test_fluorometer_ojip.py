import time
import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT
from .fluorometer_ojip_config import (
    OJIP_CAPTURE_BODY,
    OJIP_CAPTURE_TIMEOUT,
    OJIP_COMPLETED_POLL_TIMEOUT,
    OJIP_COMPLETED_POLL_INTERVAL,
)

_CAPTURE_URL   = "/sensor/fluorometer/ojip/capture"
_COMPLETED_URL = "/sensor/fluorometer/ojip/completed"
_READ_LAST_URL = "/sensor/fluorometer/ojip/read_last"


# ==========================================
# Fluorometer OJIP workflow
# ==========================================


@pytest.fixture(scope="module")
def ojip_capture_result():
    resp = requests.post(
        f"{BASE_URL}{_CAPTURE_URL}",
        json=OJIP_CAPTURE_BODY,
        timeout=OJIP_CAPTURE_TIMEOUT,
    )
    assert resp.status_code == 200, (
        f"POST {_CAPTURE_URL} returned {resp.status_code}: {resp.text}"
    )
    capture_data = resp.json()
    assert "samples" in capture_data, (
        f"POST {_CAPTURE_URL} response missing 'samples': {capture_data}"
    )
    return capture_data

@pytest.fixture(scope="module")
def wait_for_capture_complete():
    deadline = time.monotonic() + OJIP_COMPLETED_POLL_TIMEOUT
    while True:
        resp = requests.get(
            f"{BASE_URL}{_COMPLETED_URL}",
            timeout=REQUEST_TIMEOUT,
        )
        assert resp.status_code == 200, (
            f"GET {_COMPLETED_URL} returned {resp.status_code}"
        )
        if resp.json().get("capture_complete") is True:
            break
        assert time.monotonic() < deadline, (
            f"GET {_COMPLETED_URL}: capture_complete did not become true "
            f"within {OJIP_COMPLETED_POLL_TIMEOUT}s"
        )
        time.sleep(OJIP_COMPLETED_POLL_INTERVAL)

@pytest.fixture(scope="module")
def first_read_last(wait_for_capture_complete):
    resp = requests.get(f"{BASE_URL}{_READ_LAST_URL}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, (
        f"GET {_READ_LAST_URL} (1st) returned {resp.status_code}"
    )
    data_first = resp.json()
    assert "samples" in data_first, (
        f"GET {_READ_LAST_URL} (1st) response missing 'samples': {data_first}"
    )
    assert data_first.get("read") is False, (
        f"GET {_READ_LAST_URL} (1st): expected read=false, got {data_first.get('read')}"
    )
    return data_first

@pytest.fixture(scope="module")
def second_read_last(first_read_last):
    resp = requests.get(f"{BASE_URL}{_READ_LAST_URL}", timeout=REQUEST_TIMEOUT)
    assert resp.status_code == 200, (
        f"GET {_READ_LAST_URL} (2nd) returned {resp.status_code}"
    )
    data_second = resp.json()
    assert data_second.get("read") is True, (
        f"GET {_READ_LAST_URL} (2nd): expected read=true, got {data_second.get('read')}"
    )
    return data_second


def test_ojip_capture(ojip_capture_result):
    assert "samples" in ojip_capture_result

def test_ojip_completed(wait_for_capture_complete):
    pass 

def test_ojip_first_read_last(first_read_last):
    assert first_read_last.get("read") is False

def test_ojip_second_read_last(first_read_last, second_read_last):
    assert second_read_last.get("read") is True
    assert second_read_last.get("measurement_id") == first_read_last.get("measurement_id")
