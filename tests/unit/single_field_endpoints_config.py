"""
Configuration for POST endpoints with single numeric value.
Each endpoint is tested with the same test suite, only ranges differ.
"""

SINGLE_FIELD_ENDPOINTS = [
    {
        "name": "setHeaterIntensity",
        "url": "/control/heater/intensity",
        "field": "intensity",
        "min": -1.0,
        "max": 1.0,
    },
    {
        "name": "setCuvettePumpSpeed",
        "url": "/control/cuvette_pump/speed",
        "field": "speed",
        "min": -1.0,
        "max": 1.0,
    },
    {
        "name": "setCuvettePumpFlowrate",
        "url": "/control/cuvette_pump/flowrate",
        "field": "flowrate",
        "min": -1000.0,
        "max": 1000.0,
    },
]
