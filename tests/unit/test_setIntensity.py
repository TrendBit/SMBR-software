"""
Unit tests for POST /control/led_panel/intensity/{channel} endpoint
All LED channels will be set to 0 at the end of tests
"""
import pytest
import requests
from ..config import BASE_URL

BASE_ENDPOINT = f"{BASE_URL}/control/led_panel/intensity"


class TestSetIntensity:
    """Tests for POST /control/led_panel/intensity/{channel}"""
    
    # Valid Requests (Expected: 200 OK)
    
    def test_channel0_minimum_intensity(self):
        """Valid: Channel 0 - minimum intensity (0.0)"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.0})
        assert response.status_code == 200
        assert "successful" in response.json()["message"]
    
    def test_channel0_maximum_intensity(self):
        """Valid: Channel 0 - maximum intensity (1.0)"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 1.0})
        assert response.status_code == 200
    
    def test_channel0_middle_intensity(self):
        """Valid: Channel 0 - middle intensity (0.5)"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.5})
        assert response.status_code == 200
    
    def test_channel0_decimal_value(self):
        """Valid: Channel 0 - decimal value"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.123456})
        assert response.status_code == 200
    
    @pytest.mark.parametrize("channel,intensity", [
        (1, 0.75),
        (2, 0.25),
        (3, 0.999),
    ])
    def test_all_channels_with_different_values(self, channel, intensity):
        """Valid: Test all channels (1, 2, 3) with different values"""
        response = requests.post(f"{BASE_ENDPOINT}/{channel}", json={"intensity": intensity})
        assert response.status_code == 200
    
    def test_integer_value_zero(self):
        """Valid: Integer value 0 (converted to float)"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0})
        assert response.status_code == 200
    
    def test_integer_value_one(self):
        """Valid: Integer value 1 (converted to float)"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 1})
        assert response.status_code == 200
    
    def test_very_small_positive_number(self):
        """Valid: Very small positive number"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.0001})
        assert response.status_code == 200
    
    def test_number_very_close_to_one(self):
        """Valid: Number very close to 1"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.9999999})
        assert response.status_code == 200
    
    def test_very_long_decimal_precision(self):
        """Valid: Very long decimal precision"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.123456789012345678})
        assert response.status_code == 200
    
    # Errors Caught by Code Validation or ErrorHandler (Expected: 400)
    
    def test_empty_body(self):
        """Error: Empty body"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={})
        assert response.status_code == 400
    
    def test_missing_intensity_field(self):
        """Error: Missing 'intensity' field"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"other": 0.5})
        assert response.status_code == 400
    
    def test_null_intensity_value(self):
        """Error: Null intensity value"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": None})
        assert response.status_code == 400
    
    @pytest.mark.parametrize("intensity", [
        -0.1,      # negative
        1.1,       # greater than 1
        -100,      # large negative
        1000,      # large positive
        5.761053081236682e+16,  # very large number
        -0.0000001,  # very small negative (boundary)
        1.0000001,   # just above 1.0 (boundary)
    ])
    def test_values_out_of_range(self, intensity):
        """Error: Values outside 0-1 range"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": intensity})
        assert response.status_code == 400
    
    @pytest.mark.parametrize("special_value", [
        "Infinity",
        "-Infinity",
        "NaN",
    ])
    def test_special_float_values(self, special_value):
        """Error: Special float values (Infinity, -Infinity, NaN)"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data=f'{{"intensity": {special_value}}}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 400
    
    @pytest.mark.parametrize("wrong_type", [
        "text",           # string
        [0.5],            # array
        {"value": 0.5},   # object
    ])
    def test_wrong_type_instead_of_number(self, wrong_type):
        """Error: Wrong type instead of number"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": wrong_type})
        assert response.status_code == 400
    
    def test_invalid_json(self):
        """Error: Invalid JSON"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data="{invalid json}",
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 400
    
    def test_malformed_json_missing_quotes(self):
        """Error: Malformed JSON - missing quotes"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data="{intensity: 0.5}",
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 400
    
    def test_wrong_content_type_header(self):
        """Error: Wrong Content-Type header"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data='{"intensity": 0.5}',
            headers={"Content-Type": "text/plain"}
        )
        assert response.status_code == 400
    
    def test_empty_request_body(self):
        """Error: Empty request body (no JSON at all)"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 400
    
    # Invalid Channel Parameters (Expected: 400)
    
    @pytest.mark.parametrize("channel", [
        -1,          # negative
        4,           # too high
        "channel0",  # text
        "1.5",       # float as string
        "",          # empty
    ])
    def test_invalid_channel_parameter(self, channel):
        """Error: Invalid channel parameter"""
        response = requests.post(f"{BASE_ENDPOINT}/{channel}", json={"intensity": 0.5})
        assert response.status_code == 400
    
    # Framework Limitations (Expected: 200 OK)
    
    def test_boolean_false_converted_to_zero(self):
        """Framework Limitation: Boolean false converted to 0.0"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data='{"intensity": false}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200
    
    def test_boolean_true_converted_to_one(self):
        """Framework Limitation: Boolean true converted to 1.0"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data='{"intensity": true}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200
    
    def test_additional_property_simple(self):
        """Framework Limitation: Additional property with simple value"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.5, "extra": "value"})
        assert response.status_code == 200
    
    def test_additional_property_complex(self):
        """Framework Limitation: Additional property with complex value"""
        response = requests.post(f"{BASE_ENDPOINT}/0", json={"intensity": 0.5, "extra": {"nested": True}})
        assert response.status_code == 200
    
    def test_duplicate_keys(self):
        """Framework Limitation: Duplicate keys (last value used)"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data='{"intensity": 0.0, "intensity": 1.0}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200
    
    def test_trailing_comma_accepted(self):
        """Framework Limitation: Trailing comma accepted"""
        response = requests.post(
            f"{BASE_ENDPOINT}/0",
            data='{"intensity": 0.5,}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200

def test_cleanup_set_all_channels_to_zero():
    """Cleanup: Set all LED channels to 0 at the end"""
    for channel in range(4):
        response = requests.post(f"{BASE_ENDPOINT}/{channel}", json={"intensity": 0})
        assert response.status_code == 200
    print("\n✓ All LED channels reset to 0")
