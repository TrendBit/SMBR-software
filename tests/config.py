"""
Test configuration - Device connection settings
"""

# Device connection
DEVICE_IP = "192.168.0.244"
DEVICE_PORT = 8089
BASE_URL = f"http://{DEVICE_IP}:{DEVICE_PORT}"

# Test execution settings
REQUEST_TIMEOUT = 10  # Default timeout for all requests in seconds
REQUEST_DELAY = 1  # Delay between requests in seconds
