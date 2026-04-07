LED_PANEL_INTENSITY_URL   = "/control/led_panel/intensity"
LED_PANEL_TEMP_URL        = "/control/led_panel/temperature"
LED_PANEL_ZERO_POST       = {"post_url": LED_PANEL_INTENSITY_URL, "field": "intensity", "value": [0.0, 0.0, 0.0, 0.0]}

FLUOROMETER_SINGLE_URL    = "/sensor/fluorometer/single_sample"
FLUOROMETER_EMITOR_URL    = "/sensor/fluorometer/emitor/temperature"
FLUOROMETER_DETECTOR_URL  = "/sensor/fluorometer/detector/temperature"

SPECTRO_MEASURE_URL               = "/sensor/spectrophotometer/measure_all"
SPECTRO_EMITOR_URL                = "/sensor/spectrophotometer/emitor/temperature"

# ── LED panel ────────────────────────────────────────────────

# Intensity sent to the LED panel (all channels)
LED_FULL_INTENSITY        = [1.0, 1.0, 1.0, 1.0]

# Seconds to run at full intensity before measuring temperature rise
LED_HEAT_WAIT_S           = 30.0

# Minimum expected temperature rise (°C) while running at full intensity
LED_RISE_MIN_DELTA_C      = 3.0 

# Seconds to wait after zeroing out LED before checking cooldown
LED_COOL_WAIT_S           = 60.0

# Minimum expected temperature drop (°C) after LED is zeroed out
LED_COOL_MIN_DELTA_C      = 3.0 

# ── Fluorometer emitor ────────────────────────────────────────────────

# Number of rapid single_sample calls to heat up the emitor
FLUORO_SAMPLE_COUNT       = 20

# Parameters for each single_sample call
FLUORO_DETECTOR_GAIN      = "x1"
FLUORO_EMITOR_INTENSITY   = 1.0

# Minimum expected temperature rise (°C) in emitor after rapid sampling
FLUORO_EMITOR_RISE_MIN_DELTA_C    = 1.5

# Seconds to wait after sampling stops before checking that emitor cooled
FLUORO_EMITOR_COOL_WAIT_S         = 60.0

# Minimum expected temperature drop (°C) in emitor after idle period
FLUORO_EMITOR_COOL_MIN_DELTA_C    = 1.5

# ── Spectrophotometer emitor ────────────────────────────────────────────────

# Number of rapid measure_all calls to heat up the emitor
SPECTRO_SAMPLE_COUNT              = 10

# Minimum expected temperature rise (°C) in spectrophotometer emitor after rapid measurement
SPECTRO_EMITOR_RISE_MIN_DELTA_C   = 0.5

# Seconds to wait after measurement stops before checking that emitor cooled
SPECTRO_EMITOR_COOL_WAIT_S        = 60.0

# Minimum expected temperature drop (°C) in spectrophotometer emitor after idle period
SPECTRO_EMITOR_COOL_MIN_DELTA_C   = 0.5
