
# Seconds to wait after issuing move before checking that motion is in progress
MOVE_SETTLE_S = 0.5

# ── Move + completion scenarios ───────────────────────────────────────────────
# Short move: POST → check flowrate ≠ 0 (in progress) → wait → check flowrate = 0 (done)
MOVE_COMPLETE_SCENARIOS = [
    {
        "name":                "cuvette_pump_move_completes",
        "move_url":            "/control/cuvette_pump/move",
        "move_body":           {"volume": 5.0, "flowrate": 50.0}, 
        "get_url":             "/control/cuvette_pump/flowrate",
        "get_field":           "flowrate",
        "stop_url":            "/control/cuvette_pump/stop",
        "expected_duration_s": 10.0,  
    },
]

# ── Move + interruption scenarios ─────────────────────────────────────────────
# Large move: POST → immediately stop → check that flowrate = 0 
MOVE_INTERRUPT_SCENARIOS = [
    {
        "name":      "aerator_move_interrupted_by_stop",
        "move_url":  "/control/aerator/move",
        "move_body": {"volume": 500.0, "flowrate": 100.0},  
        "get_url":   "/control/aerator/flowrate",
        "get_field": "flowrate",
        "stop_url":  "/control/aerator/stop",
    },
]

# ── Mixer stir + completion scenario ─────────────────────────────────────────
# POST stir with short duration → wait until duration elapses → check speed = 0
STIR_COMPLETE_SCENARIOS = [
    {
        "name":     "mixer_stir_completes_after_duration",
        "stir_url": "/control/mixer/stir",
        "stir_body": {"rpm": 1000, "time": 3}, 
        "get_url":  "/control/mixer/speed",
        "get_field": "speed",
        "wait_s":   6.0,   
        "stop_url": "/control/mixer/stop",
    },
]

# ── Pumps move + interruption ─────────────────────────────────────────────────
# Dynamically discovered instances; large volume so stop always interrupts
PUMPS_MOVE_BODY = {"volume": 500.0, "flowrate": 50.0} 
