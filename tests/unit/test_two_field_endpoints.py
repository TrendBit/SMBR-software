"""
Parametrized tests for two field POST endpoints with numeric values.
Tests all endpoints defined in two_field_endpoints_config.py with the same test suite.
"""

import pytest
import requests
import time
from .two_field_endpoints_config import TWO_FIELD_ENDPOINTS
from ..config import BASE_URL, REQUEST_TIMEOUT, REQUEST_DELAY


@pytest.fixture(scope="session")
def base_url():
    """Base URL for the API server"""
    return BASE_URL

_last_endpoint = {"name": None}


def post_with_delay(*args, **kwargs):
    """Wrapper around requests.post() that adds delay after each request"""
    response = requests.post(*args, **kwargs)
    time.sleep(REQUEST_DELAY)
    return response


def cleanup_endpoint(base_url, endpoint):
    """Reset endpoint to safe state"""
    try:
        # Primary cleanup: set to safe values
        safe_value1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        if endpoint["field1_min"] <= 0 <= endpoint["field1_max"]:
            safe_value1 = 0.0
        
        safe_value2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        if endpoint["field2_min"] <= 0 <= endpoint["field2_max"]:
            safe_value2 = 0.0
        
        requests.post(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: safe_value1,
                endpoint["field2"]: safe_value2
            },
            timeout=1
        )
        time.sleep(REQUEST_DELAY)
        
        # Additional cleanup: use dedicated cleanup endpoint if available
        if "cleanup_url" in endpoint:
            requests.get(
                f"{base_url}{endpoint['cleanup_url']}",
                timeout=1
            )
            time.sleep(REQUEST_DELAY)
    except Exception:
        pass  


class TestTwoFieldEndpoints:
    """Parametrized test suite for two field POST endpoints"""
    
    @pytest.fixture(autouse=True, params=TWO_FIELD_ENDPOINTS, ids=lambda e: e["name"], scope="class")
    def endpoint(self, request, base_url):
        """Provide endpoint configuration and handle cleanup"""
        endpoint_config = request.param
        
        if _last_endpoint["name"] is not None and _last_endpoint["name"] != endpoint_config["name"]:
            cleanup_endpoint(base_url, _last_endpoint["endpoint"])
        
        _last_endpoint["name"] = endpoint_config["name"]
        _last_endpoint["endpoint"] = endpoint_config
        
        yield endpoint_config
        
        def final_cleanup():
            if _last_endpoint["name"] == endpoint_config["name"]:
                cleanup_endpoint(base_url, endpoint_config)
        
        request.addfinalizer(final_cleanup)
    
    # ==========================================
    # Valid Requests (200 OK)
    # ==========================================
    
    def test_valid_both_minimum(self, base_url, endpoint):
        """Test both fields at minimum values"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_min"],
                endpoint["field2"]: endpoint["field2_min"]
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_both_maximum(self, base_url, endpoint):
        """Test both fields at maximum values"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_max"],
                endpoint["field2"]: endpoint["field2_max"]
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_both_zero(self, base_url, endpoint):
        """Test both fields at zero if within range"""
        if (endpoint["field1_min"] <= 0.0 <= endpoint["field1_max"] and 
            endpoint["field2_min"] <= 0.0 <= endpoint["field2_max"]):
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={
                    endpoint["field1"]: 0.0,
                    endpoint["field2"]: 0.0
                },
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_both_midpoint(self, base_url, endpoint):
        """Test both fields at midpoint values"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_field1_min_field2_max(self, base_url, endpoint):
        """Test field1 at minimum, field2 at maximum"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_min"],
                endpoint["field2"]: endpoint["field2_max"]
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_field1_max_field2_min(self, base_url, endpoint):
        """Test field1 at maximum, field2 at minimum"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_max"],
                endpoint["field2"]: endpoint["field2_min"]
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_high_precision(self, base_url, endpoint):
        """Test high precision decimal values"""
        value1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        value2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        precise_value1 = round(value1 + 0.123456789, 9)
        precise_value2 = round(value2 + 0.987654321, 9)
        
        if (precise_value1 >= endpoint["field1_min"] and precise_value1 <= endpoint["field1_max"] and
            precise_value2 >= endpoint["field2_min"] and precise_value2 <= endpoint["field2_max"]):
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={
                    endpoint["field1"]: precise_value1,
                    endpoint["field2"]: precise_value2
                },
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_scientific_notation(self, base_url, endpoint):
        """Test scientific notation within range"""
        value1 = 5.0e1  # 50.0
        value2 = 1.5e2  # 150.0
        if (value1 >= endpoint["field1_min"] and value1 <= endpoint["field1_max"] and
            value2 >= endpoint["field2_min"] and value2 <= endpoint["field2_max"]):
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={
                    endpoint["field1"]: value1,
                    endpoint["field2"]: value2
                },
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_integer_values(self, base_url, endpoint):
        """Test integer values (converted to float)"""
        int_val1 = int((endpoint["field1_min"] + endpoint["field1_max"]) / 2)
        int_val2 = int((endpoint["field2_min"] + endpoint["field2_max"]) / 2)
        
        if (int_val1 >= endpoint["field1_min"] and int_val1 <= endpoint["field1_max"] and
            int_val2 >= endpoint["field2_min"] and int_val2 <= endpoint["field2_max"]):
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={
                    endpoint["field1"]: int_val1,
                    endpoint["field2"]: int_val2
                },
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_mixed_int_float(self, base_url, endpoint):
        """Test mixed integer and float values"""
        int_val = int((endpoint["field1_min"] + endpoint["field1_max"]) / 2)
        float_val = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        
        if (int_val >= endpoint["field1_min"] and int_val <= endpoint["field1_max"] and
            float_val >= endpoint["field2_min"] and float_val <= endpoint["field2_max"]):
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={
                    endpoint["field1"]: int_val,
                    endpoint["field2"]: float_val
                },
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    # ==========================================
    # Error Cases (400 Bad Request)
    # ==========================================
    
    def test_error_empty_body(self, base_url, endpoint):
        """Test empty request body"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "required" in response.json()["message"].lower()
    
    def test_error_missing_field1(self, base_url, endpoint):
        """Test missing first field"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field2"]: midpoint2},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "required" in response.json()["message"].lower()
    
    def test_error_missing_field2(self, base_url, endpoint):
        """Test missing second field"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field1"]: midpoint1},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "required" in response.json()["message"].lower()
    
    def test_error_null_field1(self, base_url, endpoint):
        """Test null value in first field"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: None,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        message = response.json()["message"].lower()
        assert "required" in message or "malformed" in message or "invalid" in message
    
    def test_error_null_field2(self, base_url, endpoint):
        """Test null value in second field"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: None
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        message = response.json()["message"].lower()
        assert "required" in message or "malformed" in message or "invalid" in message
    
    def test_error_both_null(self, base_url, endpoint):
        """Test both fields null"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: None,
                endpoint["field2"]: None
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        message = response.json()["message"].lower()
        assert "required" in message or "malformed" in message or "invalid" in message
    
    def test_error_missing_body(self, base_url, endpoint):
        """Test completely missing request body"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        message = response.json()["message"].lower()
        assert "required" in message or "malformed" in message or "invalid" in message
    
    def test_error_field1_below_minimum(self, base_url, endpoint):
        """Test field1 below minimum"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_min"] - 0.001,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_field1_above_maximum(self, base_url, endpoint):
        """Test field1 above maximum"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_max"] + 0.001,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_field2_below_minimum(self, base_url, endpoint):
        """Test field2 below minimum"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: endpoint["field2_min"] - 0.001
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_field2_above_maximum(self, base_url, endpoint):
        """Test field2 above maximum"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: endpoint["field2_max"] + 0.001
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_both_below_minimum(self, base_url, endpoint):
        """Test both fields below minimum"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_min"] - 1.0,
                endpoint["field2"]: endpoint["field2_min"] - 1.0
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_both_above_maximum(self, base_url, endpoint):
        """Test both fields above maximum"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: endpoint["field1_max"] + 1.0,
                endpoint["field2"]: endpoint["field2_max"] + 1.0
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field1_string(self, base_url, endpoint):
        """Test string value for field1"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: "10.5",
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field2_string(self, base_url, endpoint):
        """Test string value for field2"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: "20.5"
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_both_strings(self, base_url, endpoint):
        """Test both fields as strings"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: "10.5",
                endpoint["field2"]: "20.5"
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field1_array(self, base_url, endpoint):
        """Test array value for field1"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: [10.5],
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field2_object(self, base_url, endpoint):
        """Test object value for field2"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: {"value": 20.5}
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_very_large_field1(self, base_url, endpoint):
        """Test very large value for field1"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: 1e10,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_very_large_field2(self, base_url, endpoint):
        """Test very large value for field2"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: 1e10
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_malformed_json_missing_brace(self, base_url, endpoint):
        """Test malformed JSON - missing closing brace"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": {midpoint2}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_invalid_json(self, base_url, endpoint):
        """Test completely invalid JSON"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data="{invalid json}",
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_malformed_json_single_quotes(self, base_url, endpoint):
        """Test malformed JSON - single quotes"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f"{{'{endpoint['field1']}': {midpoint1}, '{endpoint['field2']}': {midpoint2}}}",
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_malformed_json_unquoted(self, base_url, endpoint):
        """Test malformed JSON - unquoted field names"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{{endpoint["field1"]}: {midpoint1}, {endpoint["field2"]}: {midpoint2}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field1_infinity(self, base_url, endpoint):
        """Test field1 as Infinity"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": Infinity, "{endpoint["field2"]}": {midpoint2}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field1_negative_infinity(self, base_url, endpoint):
        """Test field1 as negative Infinity"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": -Infinity, "{endpoint["field2"]}": {midpoint2}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field1_nan(self, base_url, endpoint):
        """Test field1 as NaN"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": NaN, "{endpoint["field2"]}": {midpoint2}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field2_infinity(self, base_url, endpoint):
        """Test field2 as Infinity"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": Infinity}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field2_negative_infinity(self, base_url, endpoint):
        """Test field2 as negative Infinity"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": -Infinity}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_field2_nan(self, base_url, endpoint):
        """Test field2 as NaN"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": NaN}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_wrong_content_type(self, base_url, endpoint):
        """Test wrong Content-Type header"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": {midpoint2}}}',
            headers={"Content-Type": "text/plain"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    # ==========================================
    # Framework Limitations (200 OK despite invalid data)
    # ==========================================
    
    def test_framework_additional_property(self, base_url, endpoint):
        """Test additional property (framework accepts it)"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: midpoint2,
                "extra": "value"
            },
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_framework_field1_boolean_false(self, base_url, endpoint):
        """Test boolean false for field1 (framework converts to 0.0)"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: False,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        if endpoint["field1_min"] <= 0.0 <= endpoint["field1_max"]:
            assert response.status_code == 200
        else:
            assert response.status_code == 400
    
    def test_framework_field1_boolean_true(self, base_url, endpoint):
        """Test boolean true for field1 (framework converts to 1.0)"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: True,
                endpoint["field2"]: midpoint2
            },
            timeout=REQUEST_TIMEOUT
        )
        if endpoint["field1_min"] <= 1.0 <= endpoint["field1_max"]:
            assert response.status_code == 200
        else:
            assert response.status_code == 400
    
    def test_framework_field2_boolean_false(self, base_url, endpoint):
        """Test boolean false for field2 (framework converts to 0.0)"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={
                endpoint["field1"]: midpoint1,
                endpoint["field2"]: False
            },
            timeout=REQUEST_TIMEOUT
        )
        if endpoint["field2_min"] <= 0.0 <= endpoint["field2_max"]:
            assert response.status_code == 200
        else:
            assert response.status_code == 400
    
    def test_framework_missing_comma(self, base_url, endpoint):
        """Test missing comma between fields (framework accepts it)"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1} "{endpoint["field2"]}": {midpoint2}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_framework_duplicate_keys(self, base_url, endpoint):
        """Test duplicate keys (framework uses last value)"""
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        valid_value1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": 0.0, "{endpoint["field2"]}": {midpoint2}, "{endpoint["field1"]}": {valid_value1}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_framework_trailing_comma(self, base_url, endpoint):
        """Test trailing comma in JSON (framework accepts it)"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": {midpoint2},}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200  
    
    def test_framework_missing_content_type(self, base_url, endpoint):
        """Test missing Content-Type header (framework assumes JSON by default)"""
        midpoint1 = (endpoint["field1_min"] + endpoint["field1_max"]) / 2
        midpoint2 = (endpoint["field2_min"] + endpoint["field2_max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field1"]}": {midpoint1}, "{endpoint["field2"]}": {midpoint2}}}',
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
