# ── Fluorometer OJIP capture request parameters ──────────────────────────────

OJIP_CAPTURE_BODY = {
    "detector_gain":   "x1",
    "emitor_intensity": 1.0,
    "timebase":        "logarithmic",
    "length_ms":       1000,
    "sample_count":    1000,
}

OJIP_CAPTURE_TIMEOUT = 10 

OJIP_COMPLETED_POLL_TIMEOUT = 10   
OJIP_COMPLETED_POLL_INTERVAL = 0.5 
