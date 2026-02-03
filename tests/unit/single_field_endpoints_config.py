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
        "cleanup_url": "/control/heater/turn_off",
    },
    {
        "name": "setCuvettePumpSpeed",
        "url": "/control/cuvette_pump/speed",
        "field": "speed",
        "min": -1.0,
        "max": 1.0,
        "cleanup_url": "/control/cuvette_pump/stop",
    },
    {
        "name": "setCuvettePumpFlowrate",
        "url": "/control/cuvette_pump/flowrate",
        "field": "flowrate",
        "min": -1000.0,
        "max": 1000.0,
        "cleanup_url": "/control/cuvette_pump/stop",
    },
    {
        "name": "setAeratorSpeed",
        "url": "/control/aerator/speed",
        "field": "speed",
        "min": 0.0,
        "max": 1.0,
        "cleanup_url": "/control/aerator/stop",
    },
    {
        "name": "setAeratorFlowrate",
        "url": "/control/aerator/flowrate",
        "field": "flowrate",
        "min": 10.0,
        "max": 5000.0,
        "cleanup_url": "/control/aerator/stop",
    },
    {
        "name": "setMixerSpeed",
        "url": "/control/mixer/speed",
        "field": "speed",
        "min": 0.0,
        "max": 1.0,
        "cleanup_url": "/control/mixer/stop",
    },
]
