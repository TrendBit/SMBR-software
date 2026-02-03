"""
Configuration for POST endpoints with two numeric values.
Each endpoint is tested with the same test suite, only field names and ranges differ.
"""

TWO_FIELD_ENDPOINTS = [
    {
        "name": "moveCuvettePump",
        "url": "/control/cuvette_pump/move",
        "field1": "volume",
        "field1_min": 0.0,
        "field1_max": 1000.0,
        "field2": "flowrate",
        "field2_min": -1000.0,
        "field2_max": 1000.0,
        "cleanup_url": "/control/cuvette_pump/stop",
    },
]
