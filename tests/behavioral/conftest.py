import pytest
import requests
from ..config import BASE_URL, REQUEST_TIMEOUT


@pytest.fixture(scope="module")
def pump_instances():
    """Discover all pump module instances that have at least one pump."""
    instances = {}
    for idx in range(1, 13):
        r = requests.get(f"{BASE_URL}/pumps/{idx}/pump_count", timeout=REQUEST_TIMEOUT)
        if r.status_code == 200:
            try:
                cnt = int(r.json().get("pump_count", 0))
            except Exception:
                cnt = 0
            if cnt > 0:
                instances[idx] = cnt
    return instances
