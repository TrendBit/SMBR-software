"""
Unit tests for POST /control/led_panel/intensity endpoint
LEDs will be set to [0,0,0,0] at the end of tests
"""
import pytest
import requests
from config import BASE_URL

ENDPOINT = f"{BASE_URL}/control/led_panel/intensity"


class TestSetIntensities:
    """Tests for POST /control/led_panel/intensity"""
    
    # Valid Requests (Expected: 200 OK)
    
    def test_correct_format(self):
        """Valid: Correct format with 4 values"""
        response = requests.post(ENDPOINT, json={"intensity": [0.5, 1, 0, 0.2]})
        assert response.status_code == 200
        assert "successful" in response.json()["message"]
    
    def test_all_zeros(self):
        """Valid: All zeros"""
        response = requests.post(ENDPOINT, json={"intensity": [0, 0, 0, 0]})
        assert response.status_code == 200
    
    def test_all_ones(self):
        """Valid: All ones"""
        response = requests.post(ENDPOINT, json={"intensity": [1.0, 1.0, 1.0, 1.0]})
        assert response.status_code == 200
    
    def test_decimal_values(self):
        """Valid: Different decimal values"""
        response = requests.post(ENDPOINT, json={"intensity": [0.123, 0.456, 0.789, 0.999]})
        assert response.status_code == 200
    
    # Errors Caught by ErrorHandler (Expected: 400)
    
    def test_empty_body(self):
        """Error: Empty body"""
        response = requests.post(ENDPOINT, json={})
        assert response.status_code == 400
    
    def test_missing_intensity_field(self):
        """Error: Missing 'intensity' field"""
        response = requests.post(ENDPOINT, json={"other": [1, 2, 3, 4]})
        assert response.status_code == 400
    
    def test_null_instead_of_array(self):
        """Error: Null instead of array"""
        response = requests.post(ENDPOINT, json={"intensity": None})
        assert response.status_code == 400
    
    @pytest.mark.parametrize("wrong_type", [
        "text",      # string
        {"a": 1},    # object
        123,         # number
    ])
    def test_wrong_type_instead_of_array(self, wrong_type):
        """Error: Wrong type instead of array"""
        response = requests.post(ENDPOINT, json={"intensity": wrong_type})
        assert response.status_code == 400
    
    @pytest.mark.parametrize("wrong_element", [
        [0.5, "text", 0, 0.2],      # string in array
        [0.5, {"a": 1}, 0, 0.2],    # object in array
    ])
    def test_wrong_type_in_array(self, wrong_element):
        """Error: Wrong type of element in array"""
        response = requests.post(ENDPOINT, json={"intensity": wrong_element})
        assert response.status_code == 400
    
    def test_invalid_json(self):
        """Error: Invalid JSON"""
        response = requests.post(
            ENDPOINT,
            data="{invalid json}",
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 400
    
    @pytest.mark.parametrize("intensities", [
        [],                    # empty array
        [0.5],                 # 1 element
        [0.5, 1],              # 2 elements
        [0.5, 1, 0],           # 3 elements
        [0.5, 1, 0, 0.2, 0.3], # 5 elements
    ])
    def test_wrong_array_size(self, intensities):
        """Error: Wrong array size (not 4 elements)"""
        response = requests.post(ENDPOINT, json={"intensity": intensities})
        assert response.status_code == 400
    
    # Business Logic Errors (Expected: 500)
    
    def test_null_element_in_array(self):
        """Business Logic Error: Null element in array (correct size, but null value)"""
        response = requests.post(
            ENDPOINT,
            data='{"intensity": [null, 1, 0, 0.2]}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 500
    
    def test_all_nulls(self):
        """Business Logic Error: All nulls"""
        response = requests.post(
            ENDPOINT,
            data='{"intensity": [null, null, null, null]}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 500
    
    @pytest.mark.parametrize("intensities", [
        [-0.1, 1, 0, 0.2],      # negative value
        [0.5, 1.1, 0, 0.2],     # value > 1
        [-1, 2, -0.5, 1.5],     # all out of range
    ])
    def test_values_out_of_range(self, intensities):
        """Business Logic Error: Values outside 0-1 range"""
        response = requests.post(ENDPOINT, json={"intensity": intensities})
        assert response.status_code == 500
    
    # Framework Limitations (Expected: 200 OK)
    
    def test_additional_property_simple(self):
        """Framework Limitation: Additional property with simple value"""
        response = requests.post(ENDPOINT, json={"intensity": [0.5, 1, 0, 0.2], "extra": "value"})
        assert response.status_code == 200
    
    def test_additional_property_complex(self):
        """Framework Limitation: Additional property with complex value"""
        response = requests.post(ENDPOINT, json={"intensity": [0.5, 1, 0, 0.2], "extra": {"nested": True}})
        assert response.status_code == 200
    
    def test_duplicate_keys(self):
        """Framework Limitation: Duplicate keys (last value used)"""
        response = requests.post(
            ENDPOINT,
            data='{"intensity": [0,0,0,0], "intensity": [1,1,1,1]}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200
    
    def test_boolean_values(self):
        """Framework Limitation: Boolean values converted to numbers (false->0, true->1)"""
        response = requests.post(
            ENDPOINT,
            data='{"intensity": [false, true, true, false]}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200


def test_cleanup_set_leds_to_zero():
    """Cleanup: Set all LED intensities to 0 at the end"""
    response = requests.post(ENDPOINT, json={"intensity": [0, 0, 0, 0]})
    assert response.status_code == 200
    print("\n LEDs reset to [0, 0, 0, 0]")
