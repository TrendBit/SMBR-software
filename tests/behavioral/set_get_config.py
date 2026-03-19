
LED_CHANNELS = ["0", "1", "2", "3"]

SET_GET_ENDPOINTS = [
    # --- Heater ---
    {
        "name": "heater_target_temperature",
        "post_url": "/control/heater/target_temperature",
        "get_url":  "/control/heater/target_temperature",
        "field":    "temperature",
        "values":   [20.0, 37.0, 50.0],
        "tolerance": 0.01,
        "cleanup_url": "/control/heater/turn_off",
    },
    {
        "name": "heater_intensity",
        "post_url": "/control/heater/intensity",
        "get_url":  "/control/heater/intensity",
        "field":    "intensity",
        "values":   [-1.0, 0.0, 1.0],
        "tolerance": 0.01,
        "settle_time": 1.0,  
        "cleanup_url": "/control/heater/turn_off",
    },
    # --- Cuvette pump ---
    {
        "name": "cuvette_pump_speed",
        "post_url": "/control/cuvette_pump/speed",
        "get_url":  "/control/cuvette_pump/speed",
        "field":    "speed",
        "values":   [-1.0, 0.0, 1.0],
        "tolerance": 0.01,
        "cleanup_url": "/control/cuvette_pump/stop",
    },
    {
        "name": "cuvette_pump_flowrate",
        "post_url": "/control/cuvette_pump/flowrate",
        "get_url":  "/control/cuvette_pump/flowrate",
        "field":    "flowrate",
        "values":   [-60.0, 100.0],
        "tolerance": 0.1,
        "cleanup_url": "/control/cuvette_pump/stop",
    },
    # --- Aerator ---
    {
        "name": "aerator_speed",
        "post_url": "/control/aerator/speed",
        "get_url":  "/control/aerator/speed",
        "field":    "speed",
        "values":   [0.0, 0.5, 1.0],
        "tolerance": 0.01,
        "cleanup_url": "/control/aerator/stop",
    },
    {
        "name": "aerator_flowrate",
        "post_url": "/control/aerator/flowrate",
        "get_url":  "/control/aerator/flowrate",
        "field":    "flowrate",
        "values":   [200.0, 2000.0],
        "tolerance": 1.0,
        "cleanup_url": "/control/aerator/stop",
    },
    # --- Mixer ---
    {
        "name": "mixer_speed",
        "post_url": "/control/mixer/speed",
        "get_url":  "/control/mixer/speed",
        "field":    "speed",
        "values":   [0.0, 0.5, 1.0],
        "tolerance": 0.01,
        "cleanup_url": "/control/mixer/stop",
    },
    # {
    #     "name": "mixer_rpm",
    #     "post_url": "/control/mixer/rpm",
    #     "get_url":  "/control/mixer/rpm",
    #     "field":    "rpm",
    #     "values":   [0, 3000],
    #     "tolerance": None, 
    #     "rel":       0.1, # relative tolerance
    #     "rpm_zero_abs": 10,  # absolute tolerance when value == 0
    #     "settle_time": 10.0,  # seconds to wait after SET before GET
    #     "cleanup_url": "/control/mixer/stop",
    # },
]

# LED single-channel: channels and values tested in parametrize
LED_SINGLE_CHANNEL_VALUES = [0.0, 0.5, 1.0]

# LED all-channels: list of full intensity vectors to test
LED_ALL_CHANNEL_VALUES = [
    [0.0, 0.0, 0.0, 0.0],
    [1.0, 1.0, 1.0, 1.0],
    [0.2, 0.4, 0.6, 0.8],
]

# Pumps module: speed and flowrate values
PUMPS_SPEED_VALUES    = [-1.0, 0.0, 1.0]
PUMPS_FLOWRATE_VALUES = [28.0]
