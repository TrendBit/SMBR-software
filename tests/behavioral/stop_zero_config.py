
STOP_ZERO_ENDPOINTS = [
    {
        "name": "cuvette_pump",
        "activate": [
            {"post_url": "/control/cuvette_pump/speed", "field": "speed", "value": 1.0},
            {"post_url": "/control/cuvette_pump/flowrate", "field": "flowrate", "value": 100.0},
        ],
        "stop_url": "/control/cuvette_pump/stop",
        "verify": [
            {"get_url": "/control/cuvette_pump/speed",    "field": "speed",    "expected": 0.0, "tolerance": 0.01},
            {"get_url": "/control/cuvette_pump/flowrate", "field": "flowrate", "expected": 0.0, "tolerance": 1.0},
        ],
    },
    {
        "name": "aerator",
        "activate": [
            {"post_url": "/control/aerator/speed", "field": "speed", "value": 1.0},
            {"post_url": "/control/aerator/flowrate", "field": "flowrate", "value": 100.0},
        ],
        "stop_url": "/control/aerator/stop",
        "verify": [
            {"get_url": "/control/aerator/speed",    "field": "speed",    "expected": 0.0, "tolerance": 0.01},
            {"get_url": "/control/aerator/flowrate", "field": "flowrate", "expected": 0.0, "tolerance": 1.0},
        ],
    },
    {
        "name": "mixer",
        "activate": [
            {"post_url": "/control/mixer/speed", "field": "speed", "value": 1.0},
        ],
        "stop_url": "/control/mixer/stop",
        "settle_time": 5.0,  # wait for physical rotor to spin down
        "verify": [
            {"get_url": "/control/mixer/speed", "field": "speed", "expected": 0.0, "tolerance": 0.01},
            {"get_url": "/control/mixer/rpm",   "field": "rpm",   "expected": 0,   "tolerance": 10},
        ],
    },
    {
        "name": "heater",
        "activate": [
            {"post_url": "/control/heater/intensity",          "field": "intensity",   "value": 0.5},
            {"post_url": "/control/heater/target_temperature", "field": "temperature", "value": 37.0},
        ],
        "stop_url": "/control/heater/turn_off",
        "settle_time": 1.0,
        "verify": [
            {"get_url": "/control/heater/intensity",          "field": "intensity",   "expected": 0.0,  "tolerance": 0.01},
            {"get_url": "/control/heater/target_temperature", "field": "temperature", "expected": None},
        ],
    },
]

# Speed value used to activate a pump before testing stop behaviour
PUMPS_ACTIVATION_SPEED = 1.0
