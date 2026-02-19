"""
Parametrized tests for single field POST endpoints with numeric value.
Tests all endpoints defined in single_field_endpoints_config.py with the same test suite.
"""

import pytest
import requests
import time
from .single_field_endpoints_config import SINGLE_FIELD_ENDPOINTS
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
        # Primary cleanup: set to safe value
        safe_value = (endpoint["min"] + endpoint["max"]) / 2
        if endpoint["min"] <= 0 <= endpoint["max"]:
            safe_value = 0.0
        
        requests.post(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: safe_value},
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


class TestSingleFieldEndpoints:
    """Parametrized test suite for single field POST endpoints"""
    
    @pytest.fixture(autouse=True, params=SINGLE_FIELD_ENDPOINTS, ids=lambda e: e["name"], scope="class")
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
    
    def test_valid_minimum(self, base_url, endpoint):
        """Test minimum valid value"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: endpoint["min"]},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_maximum(self, base_url, endpoint):
        """Test maximum valid value"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: endpoint["max"]},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_zero(self, base_url, endpoint):
        """Test zero value if within range"""
        if endpoint["min"] <= 0.0 <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: 0.0},
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_midpoint(self, base_url, endpoint):
        """Test midpoint value"""
        midpoint = (endpoint["min"] + endpoint["max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: midpoint},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_valid_small_positive(self, base_url, endpoint):
        """Test very small positive value"""
        if 0.0001 >= endpoint["min"] and 0.0001 <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: 0.0001},
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_small_negative(self, base_url, endpoint):
        """Test very small negative value"""
        if -0.0001 >= endpoint["min"] and -0.0001 <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: -0.0001},
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_high_precision(self, base_url, endpoint):
        """Test high precision decimal value"""
        value = (endpoint["min"] + endpoint["max"]) / 2
        precise_value = round(value + 0.123456789, 9)
        if precise_value >= endpoint["min"] and precise_value <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: precise_value},
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_scientific_notation(self, base_url, endpoint):
        """Test scientific notation within range"""
        value = 5.0e-1  
        if value >= endpoint["min"] and value <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: value},
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_integer_zero(self, base_url, endpoint):
        """Test integer value 0 (converted to float)"""
        if endpoint["min"] <= 0 <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: 0},
                timeout=REQUEST_TIMEOUT
            )
            assert response.status_code == 200
    
    def test_valid_integer_one(self, base_url, endpoint):
        """Test integer value 1 (converted to float)"""
        if endpoint["min"] <= 1 <= endpoint["max"]:
            response = post_with_delay(
                f"{base_url}{endpoint['url']}",
                json={endpoint["field"]: 1},
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
    
    def test_error_null_value(self, base_url, endpoint):
        """Test null value in field"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: None},
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
    
    def test_error_below_minimum(self, base_url, endpoint):
        """Test value slightly below minimum"""
        value = endpoint["min"] - 0.001
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: value},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_above_maximum(self, base_url, endpoint):
        """Test value slightly above maximum"""
        value = endpoint["max"] + 0.001
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: value},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_far_below_minimum(self, base_url, endpoint):
        """Test value far below minimum"""
        value = endpoint["min"] - 100.0
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: value},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_far_above_maximum(self, base_url, endpoint):
        """Test value far above maximum"""
        value = endpoint["max"] + 100.0
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: value},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
        assert "between" in response.json()["message"].lower() or "invalid" in response.json()["message"].lower()
    
    def test_error_very_large_positive(self, base_url, endpoint):
        """Test very large positive number"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: 1e10},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_very_large_negative(self, base_url, endpoint):
        """Test very large negative number"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: -6.68e16},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_string_value(self, base_url, endpoint):
        """Test string value instead of number"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: "0.5"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_array_value(self, base_url, endpoint):
        """Test array value instead of number"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: [0.5]},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_object_value(self, base_url, endpoint):
        """Test object value instead of number"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: {"value": 0.5}},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_infinity(self, base_url, endpoint):
        """Test positive Infinity"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": Infinity}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_negative_infinity(self, base_url, endpoint):
        """Test negative Infinity"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": -Infinity}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_nan(self, base_url, endpoint):
        """Test NaN (Not a Number)"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": NaN}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_malformed_json_missing_brace(self, base_url, endpoint):
        """Test malformed JSON - missing closing brace"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": 0.5',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_malformed_json_single_quotes(self, base_url, endpoint):
        """Test malformed JSON - single quotes"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f"{{'{endpoint['field']}': 0.5}}",
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
    
    def test_error_malformed_json_unquoted(self, base_url, endpoint):
        """Test malformed JSON - unquoted field name"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{{endpoint["field"]}: 0.5}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    def test_error_wrong_content_type(self, base_url, endpoint):
        """Test wrong Content-Type header"""
        value = (endpoint["min"] + endpoint["max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": {value}}}',
            headers={"Content-Type": "text/plain"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 400
    
    # ==========================================
    # Framework Limitations (200 OK despite invalid data)
    # ==========================================
    
    def test_framework_boolean_false(self, base_url, endpoint):
        """Test boolean false (framework converts to 0.0)"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: False},
            timeout=REQUEST_TIMEOUT
        )
        if endpoint["min"] <= 0.0 <= endpoint["max"]:
            assert response.status_code == 200
        else:
            assert response.status_code == 400
    
    def test_framework_boolean_true(self, base_url, endpoint):
        """Test boolean true (framework converts to 1.0)"""
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: True},
            timeout=REQUEST_TIMEOUT
        )
        if endpoint["min"] <= 1.0 <= endpoint["max"]:
            assert response.status_code == 200  
        else:
            assert response.status_code == 400
    
    def test_framework_additional_property_simple(self, base_url, endpoint):
        """Test additional property with simple value (framework accepts it)"""
        value = (endpoint["min"] + endpoint["max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: value, "extra": "value"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200 
    
    def test_framework_additional_property_complex(self, base_url, endpoint):
        """Test additional property with complex value (framework accepts it)"""
        value = (endpoint["min"] + endpoint["max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            json={endpoint["field"]: value, "extra": {"nested": True}},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200
    
    def test_framework_duplicate_keys(self, base_url, endpoint):
        """Test duplicate keys (framework uses last value)"""
        valid_value = (endpoint["min"] + endpoint["max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": 0.0, "{endpoint["field"]}": {valid_value}}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200  
    
    def test_framework_trailing_comma(self, base_url, endpoint):
        """Test trailing comma in JSON (may be accepted by framework)"""
        value = (endpoint["min"] + endpoint["max"]) / 2
        response = post_with_delay(
            f"{base_url}{endpoint['url']}",
            data=f'{{"{endpoint["field"]}": {value},}}',
            headers={"Content-Type": "application/json"},
            timeout=REQUEST_TIMEOUT
        )
        assert response.status_code == 200  
