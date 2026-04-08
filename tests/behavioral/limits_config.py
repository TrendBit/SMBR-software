# Seconds to wait after SET before GET so the device can update its state
SETTLE_AFTER_WRITE_S = 0.3


# ── Over-max factor for all devices ─────────────────────────────────────────
OVER_MAX_FACTOR = 1.1  # set to 1.1 for +10%

# ── Pumps limits config ──────────────────────────────────────────────────────
PUMPS_LIMITS = {
    "over_max_factor": OVER_MAX_FACTOR,  
}

AT_MAX_SCENARIOS = [
    {
        "name":        "cuvette_pump_max_flowrate",
        "info_url":    "/control/cuvette_pump/info",
        "limit_field": "max_flowrate",
        "set_url":     "/control/cuvette_pump/flowrate",
        "set_field":   "flowrate",
        "set_value":   100,         
        "get_url":     "/control/cuvette_pump/flowrate",
        "get_field":   "flowrate",
        "cleanup_url": "/control/cuvette_pump/stop",
    },
    {
        "name":        "mixer_rpm_at_max",
        "info_url":    "/control/mixer/info",
        "limit_field": "max_rpm",
        "set_url":     "/control/mixer/rpm",
        "set_field":   "rpm",
        "set_value":   6000,          
        "get_url":     "/control/mixer/rpm",
        "get_field":   "rpm",
        "cleanup_url": "/control/mixer/stop",
    },
    {
        "name":        "aerator_flowrate_at_max",
        "info_url":    "/control/aerator/info",
        "limit_field": "max_flowrate",
        "set_url":     "/control/aerator/flowrate",
        "set_field":   "flowrate",
        "set_value":   2500,          
        "get_url":     "/control/aerator/flowrate",
        "get_field":   "flowrate",
        "cleanup_url": "/control/aerator/stop",
    },
]

