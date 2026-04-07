
POWER_DRAW_URL = "/core/supply/power_draw"

# Seconds to wait before measuring baseline 
BASELINE_STABILIZE_S = 6.0

# Number of baseline samples to average (spaced 0.5 s apart)
BASELINE_SAMPLES = 4

POWER_DRAW_ENDPOINTS = [
    {
        "name": "heater",
        "activate": [
            {"post_url": "/control/heater/intensity", "field": "intensity", "value": 0.8},
        ],
        "stop_url": "/control/heater/turn_off",
        "settle_time": 3.0,
        "min_increase_w": 15.0, 
    },
    {
        "name": "led_panel",
        "activate": [
            {
                "post_url": "/control/led_panel/intensity",
                "field": "intensity",
                "value": [1.0, 1.0, 1.0, 1.0],
            },
        ],
        "stop_url": None,       
        "cleanup_post": {
            "post_url": "/control/led_panel/intensity",
            "field": "intensity",
            "value": [0.0, 0.0, 0.0, 0.0],
        },
        "settle_time": 2.0,
        "min_increase_w": 8.0,  
    },
    {
        "name": "cuvette_pump",
        "activate": [
            {"post_url": "/control/cuvette_pump/speed", "field": "speed", "value": 1.0},
        ],
        "stop_url": "/control/cuvette_pump/stop",
        "settle_time": 2.0,
        "min_increase_w": 2.0, 
    },
    {
        "name": "aerator",
        "activate": [
            {"post_url": "/control/aerator/speed", "field": "speed", "value": 1.0},
        ],
        "stop_url": "/control/aerator/stop",
        "settle_time": 2.0,
        "min_increase_w": 2.0,  
    },
    {
        "name": "mixer",
        "activate": [
            {"post_url": "/control/mixer/speed", "field": "speed", "value": 1.0},
        ],
        "stop_url": "/control/mixer/stop",
        "settle_time": 2.0,
        "min_increase_w": 0.5,  
    },
]


# Pump module power draw settings
PUMPS_ACTIVATION_SPEED  = 1.0
PUMPS_SETTLE_TIME_S     = 2.0
PUMPS_MIN_INCREASE_W    = -0.1  # TODO: Change after connecting real pump module
