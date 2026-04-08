LED_CHANNELS = ["0", "1", "2", "3"]

# Module to restart for each scenario (POST /{module}/restart)
CONTROL_MODULE = "control"

# Seconds to wait after restart before issuing verification GETs
RESTART_SETTLE_S = 5.0


STATE_PERSISTENCE_SCENARIOS = [
    # --- Heater ---
    {
        "name": "heater_target_temperature",
        "setup": [
            {"url": "/control/heater/target_temperature", "field": "temperature", "value": 35.0},
        ],
        "verify": [
            {"url": "/control/heater/target_temperature", "field": "temperature", "expected": 35.0, "tolerance": 0.01},
        ],
        "cleanup": [
            {"url": "/control/heater/turn_off", "type": "get"},
        ],
    },
    # --- Mixer ---
    {
        "name": "mixer_speed",
        "setup": [
            {"url": "/control/mixer/speed", "field": "speed", "value": 0.6},
        ],
        "verify": [
            {"url": "/control/mixer/speed", "field": "speed", "expected": 0.6, "tolerance": 0.01},
        ],
        "cleanup": [
            {"url": "/control/mixer/stop", "type": "get"},
        ],
    },
    # --- LED panel ---
    {
        "name": "led_panel_all_channels_on",
        "setup": [
            {"url": "/control/led_panel/intensity", "field": "intensity", "value": [1.0, 1.0, 1.0, 1.0]},
        ],
        "verify": [
            {"url": f"/control/led_panel/intensity/{ch}", "field": "intensity", "expected": 1.0, "tolerance": 0.01}
            for ch in LED_CHANNELS
        ],
        "cleanup": [
            {"url": "/control/led_panel/intensity", "type": "post", "body": {"intensity": [0.0, 0.0, 0.0, 0.0]},},
        ],
    },
    {
        "name": "led_panel_all_channels_off",
        "setup": [
            {"url": "/control/led_panel/intensity", "field": "intensity", "value": [0.0, 0.0, 0.0, 0.0]},
        ],
        "verify": [
            {"url": f"/control/led_panel/intensity/{ch}", "field": "intensity", "expected": 0.0, "tolerance": 0.01}
            for ch in LED_CHANNELS
        ],
        "cleanup": [],
    },
]
