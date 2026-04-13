LED_CHANNEL = "0"   # channel used for LED panel concurrency tests

# Pumps module: values used for concurrency tests
PUMPS_SPEED_VALUES        = (-0.5, 0.5)  # (value_a, value_b)
PUMPS_SPEED_IDEMPOTENT    = 0.7


# Seconds to wait after concurrent writes complete before issuing GET
SETTLE_AFTER_WRITE_S = 0.5
# Seconds to wait between first and second write (POST)
DELAY_BETWEEN_WRITES_S = 0.3

LAST_WRITE_WINS = [
    {
        "name": "heater_target_temperature",
        "post_url": "/control/heater/target_temperature",
        "get_url":  "/control/heater/target_temperature",
        "field":    "temperature",
        "value_a":  25.0,
        "value_b":  37.0,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/heater/turn_off", "type": "get"},
    },
    {
        "name": "led_panel_intensity_channel",
        "post_url": f"/control/led_panel/intensity/{LED_CHANNEL}",
        "get_url":  f"/control/led_panel/intensity/{LED_CHANNEL}",
        "field":    "intensity",
        "value_a":  0.2,
        "value_b":  0.8,
        "tolerance": 0.01,
        "cleanup": {
            "url": "/control/led_panel/intensity",
            "type": "post",
            "body": {"intensity": [0.0, 0.0, 0.0, 0.0]},
        },
    },
    {
        "name": "mixer_speed",
        "post_url": "/control/mixer/speed",
        "get_url":  "/control/mixer/speed",
        "field":    "speed",
        "value_a":  0.3,
        "value_b":  0.9,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/mixer/stop", "type": "get"},
    },
    {
        "name": "cuvette_pump_speed",
        "post_url": "/control/cuvette_pump/speed",
        "get_url":  "/control/cuvette_pump/speed",
        "field":    "speed",
        "value_a":  0.5,
        "value_b":  -0.5,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/cuvette_pump/stop", "type": "get"},
    },
    {
        "name": "aerator_flowrate",
        "post_url": "/control/aerator/flowrate",
        "get_url":  "/control/aerator/flowrate",
        "field":    "flowrate",
        "value_a":  100.0,
        "value_b":  500.0,
        "tolerance": 1.0,
        "cleanup": {"url": "/control/aerator/stop", "type": "get"},
    },
    {
        "name": "aerator_speed",
        "post_url": "/control/aerator/speed",
        "get_url":  "/control/aerator/speed",
        "field":    "speed",
        "value_a":  0.2,
        "value_b":  0.8,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/aerator/stop", "type": "get"},
    },
    {
        "name": "cuvette_pump_flowrate",
        "post_url": "/control/cuvette_pump/flowrate",
        "get_url":  "/control/cuvette_pump/flowrate",
        "field":    "flowrate",
        "value_a":  50.0,
        "value_b":  100.0,
        "tolerance": 1.0,
        "cleanup": {"url": "/control/cuvette_pump/stop", "type": "get"},
    },
]

# ── Idempotent write (same value, both writes must not corrupt state) ─────────

IDEMPOTENT_WRITES = [
    {
        "name": "heater_target_temperature",
        "post_url": "/control/heater/target_temperature",
        "get_url":  "/control/heater/target_temperature",
        "field":    "temperature",
        "value":    30.0,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/heater/turn_off", "type": "get"},
    },
    {
        "name": "mixer_speed",
        "post_url": "/control/mixer/speed",
        "get_url":  "/control/mixer/speed",
        "field":    "speed",
        "value":    0.6,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/mixer/stop", "type": "get"},
    },
    {
        "name": "led_panel_intensity_channel",
        "post_url": f"/control/led_panel/intensity/{LED_CHANNEL}",
        "get_url":  f"/control/led_panel/intensity/{LED_CHANNEL}",
        "field":    "intensity",
        "value":    0.5,
        "tolerance": 0.01,
        "cleanup": {
            "url": "/control/led_panel/intensity",
            "type": "post",
            "body": {"intensity": [0.0, 0.0, 0.0, 0.0]},
        },
    },
    {
        "name": "cuvette_pump_speed",
        "post_url": "/control/cuvette_pump/speed",
        "get_url":  "/control/cuvette_pump/speed",
        "field":    "speed",
        "value":    1.0,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/cuvette_pump/stop", "type": "get"},
    },
    {
        "name": "cuvette_pump_flowrate",
        "post_url": "/control/cuvette_pump/flowrate",
        "get_url":  "/control/cuvette_pump/flowrate",
        "field":    "flowrate",
        "value":    80.0,
        "tolerance": 1.0,
        "cleanup": {"url": "/control/cuvette_pump/stop", "type": "get"},
    },
    {
        "name": "aerator_speed",
        "post_url": "/control/aerator/speed",
        "get_url":  "/control/aerator/speed",
        "field":    "speed",
        "value":    0.5,
        "tolerance": 0.01,
        "cleanup": {"url": "/control/aerator/stop", "type": "get"},
    },
    {
        "name": "aerator_flowrate",
        "post_url": "/control/aerator/flowrate",
        "get_url":  "/control/aerator/flowrate",
        "field":    "flowrate",
        "value":    300.0,
        "tolerance": 1.0,
        "cleanup": {"url": "/control/aerator/stop", "type": "get"},
    },
]
