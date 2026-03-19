"""
Unit tests for POST /sensor/oled/print_custom_text endpoint
"""
import pytest
import requests
from ..config import BASE_URL

ENDPOINT = f"{BASE_URL}/sensor/oled/print_custom_text"


class TestPrintCustomText:
    """Tests for POST /sensor/oled/print_custom_text"""
    
    # Valid Requests (Expected: 200 OK)
    
    def test_regular_text(self):
        """Valid: Regular text"""
        response = requests.post(ENDPOINT, json={"text": "Hello World"})
        assert response.status_code == 200
    
    def test_text_with_spaces(self):
        """Valid: Text with spaces"""
        response = requests.post(ENDPOINT, json={"text": "   text with spaces   "})
        assert response.status_code == 200
    
    def test_spaces_only(self):
        """Valid: Only spaces"""
        response = requests.post(ENDPOINT, json={"text": "   "})
        assert response.status_code == 200
    
    def test_special_characters(self):
        """Valid: Special characters"""
        response = requests.post(ENDPOINT, json={"text": "Test123!@#$%"})
        assert response.status_code == 200
    
    def test_special_characters_extended(self):
        """Valid: Extended special characters"""
        response = requests.post(ENDPOINT, json={"text": "Special: @#$%^&*()"})
        assert response.status_code == 200
    
    def test_unicode_characters(self):
        """Valid: Unicode characters (Czech diacritics)"""
        response = requests.post(ENDPOINT, json={"text": "Příliš žluťoučký kůň"})
        assert response.status_code == 200
    
    def test_unicode_international(self):
        """Valid: International Unicode (Chinese, emoji)"""
        response = requests.post(ENDPOINT, json={"text": "Unicode: 你好世界 🌍"})
        assert response.status_code == 200
    
    def test_text_with_newlines(self):
        """Valid: Text with newlines"""
        response = requests.post(ENDPOINT, json={"text": "Line1\nLine2"})
        assert response.status_code == 200
    
    def test_text_with_multiple_newlines(self):
        """Valid: Text with multiple newlines"""
        response = requests.post(ENDPOINT, json={"text": "Line1\nLine2\nLine3"})
        assert response.status_code == 200
    
    def test_long_text(self):
        """Valid: Long text"""
        response = requests.post(ENDPOINT, json={"text": "This is a very long text that might overflow the display but should still be accepted by the API"})
        assert response.status_code == 200
    
    def test_very_long_text(self):
        """Valid: Very long text"""
        response = requests.post(ENDPOINT, json={"text": "x" * 127})
        assert response.status_code == 200
    
    # Errors Caught by ErrorHandler (Expected: 400)
    
    def test_empty_body(self):
        """Error: Empty body"""
        response = requests.post(ENDPOINT, json={})
        assert response.status_code == 400
    
    def test_missing_text_field(self):
        """Error: Missing 'text' field"""
        response = requests.post(ENDPOINT, json={"message": "hello"})
        assert response.status_code == 400
    
    def test_null_instead_of_string(self):
        """Error: Null instead of string"""
        response = requests.post(ENDPOINT, json={"text": None})
        assert response.status_code == 400
    
    @pytest.mark.parametrize("wrong_value", [
        123,                    # number
        True,                   # boolean
        ["hello", "world"],     # array
        {"message": "hello"}    # object
    ])
    def test_wrong_type_instead_of_string(self, wrong_value):
        """Error: Wrong type instead of string"""
        response = requests.post(ENDPOINT, json={"text": wrong_value})
        assert response.status_code == 400
    
    @pytest.mark.parametrize("invalid_json", [
        '{text: hello}',        # missing quotes
        '{"text": "hello"',     # unclosed bracket
    ])
    def test_invalid_json(self, invalid_json):
        """Error: Invalid JSON syntax"""
        response = requests.post(
            ENDPOINT,
            data=invalid_json,
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 400
    
    # Framework Limitations (Expected: 200 OK)
    
    def test_additional_property_simple(self):
        """Framework Limitation: Additional property with simple value"""
        response = requests.post(ENDPOINT, json={"text": "hello", "extra": "value"})
        assert response.status_code == 200
    
    def test_additional_property_complex(self):
        """Framework Limitation: Additional property with complex value"""
        response = requests.post(ENDPOINT, json={"text": "hello", "extra": {"nested": True}})
        assert response.status_code == 200
    
    def test_duplicate_keys(self):
        """Framework Limitation: Duplicate keys (last value used)"""
        response = requests.post(
            ENDPOINT,
            data='{"text": "first", "text": "second"}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200
    
    def test_trailing_comma(self):
        """Framework Limitation: Trailing comma (accepted by oatpp parser)"""
        response = requests.post(
            ENDPOINT,
            data='{"text": "hello",}',
            headers={"Content-Type": "application/json"}
        )
        assert response.status_code == 200
    
    # Cleanup
    
    def test_zzz_cleanup_clear_display(self):
        """Cleanup: Clear OLED display after tests"""
        clear_endpoint = f"{BASE_URL}/sensor/oled/clear_custom_text"
        response = requests.get(clear_endpoint)
        assert response.status_code == 200
