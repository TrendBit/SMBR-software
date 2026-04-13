# Unit Tests Documentation

## Overview

Unit tests verify individual API endpoints in isolation - input validation, boundary
conditions, error responses, and framework-specific behaviour. Tests are implemented using
**pytest** and are located in `tests/unit/`.

All test scenarios are parametrised; configuration values (URLs, field names, valid ranges,
expected status codes) live in separate `*_config.py` files so that no test logic file
contains hardcoded values.

---

## Test Modules

| Test file | Config file | Endpoints covered |
|-----------|-------------|-------------------|
| `test_single_field_endpoints.py` | `single_field_endpoints_config.py` | setHeaterIntensity, setHeaterTargetTemperature, setCuvettePumpSpeed, setCuvettePumpFlowrate, setAeratorSpeed, setAeratorFlowrate, setMixerSpeed, setMixerRpm |
| `test_two_field_endpoints.py` | `two_field_endpoints_config.py` | moveCuvettePump, moveAerator, stirMixer |
| `test_setIntensity.py` | — | `POST /control/led_panel/intensity/{channel}` |
| `test_setIntensities.py` | — | `POST /control/led_panel/intensity` (all channels) |
| `test_pump_endpoints.py` | — | `/pumps/{instance}/…` (pump_count, info, speed, flowrate, calibrate, move, stop) |
| `test_calibrateSensorEndpoints.py` | — | `POST /sensor/fluorometer/calibrate`, `POST /sensor/spectrophotometer/calibrate` |
| `test_printCustomText.py` | — | `POST /sensor/oled/print_custom_text` |
| `test_performFluorometerSingleSample.py` | — | `POST /sensor/fluorometer/single_sample` |
| `test_captureFluorometerOjip.py` | — | `POST /sensor/fluorometer/ojip/capture` |
| `test_getSpectrophotometerChannelInfo.py` | — | `GET /sensor/spectrophotometer/channel_info/{channel}` |
| `test_measureSingleSpectrophotometerChannel.py` | — | `POST /sensor/spectrophotometer/measure/{channel}` |

---

## Test Categories

Each test module exercises the endpoint(s) against the following categories of inputs:

### 1. Valid Requests (Expected: 200)

- Boundary values (minimum, maximum, midpoint)
- Floating-point precision (high precision, scientific notation)
- Integer values accepted in place of floats
- All valid enum values (for string-enum parameters)

### 2. Error Cases (Expected: 400)

- Missing or empty request body
- `null` field values
- Out-of-range numeric values (slightly below/above limits, far out of range)
- Wrong data types (string, array, object instead of number)
- Special float values (`Infinity`, `-Infinity`, `NaN`)
- Malformed JSON (invalid syntax, missing braces, unquoted keys)
- Wrong HTTP `Content-Type` header
- Invalid path parameters (negative, too high, non-numeric)

### 3. Framework Limitations (Expected: 200 - accepted due to oatpp)

Some inputs that should semantically be invalid are accepted by the oatpp framework:

| Behaviour | Example |
|-----------|---------|
| `false` → `0.0`, `true` → `1.0` | `{"intensity": true}` |
| Additional properties ignored | `{"intensity": 0.5, "extra": "value"}` |
| Duplicate keys - last value used | `{"intensity": 0.0, "intensity": 1.0}` |
| Trailing comma accepted | `{"intensity": 0.5,}` |
| Missing `Content-Type` header accepted | POST without `Content-Type` |
| Missing comma between fields accepted | `{"field1": 1.0 "field2": 2.0}` |

---

## Endpoint-Specific Notes

### Single Field POST Endpoints (`test_single_field_endpoints.py`)

Endpoints that accept a single numeric field with min/max validation. The full list is in
`single_field_endpoints_config.py`. All endpoints share the same test template. Only the
URL, field name, and valid range differ.

**Request format:** `{"{FIELD_NAME}": <float>}`  
**Success:** `200 OK` with `{"message": "{ENDPOINT_NAME} successful"}`  
**Error:** `400 Bad Request` with `{"message": "Invalid request body: <details>"}`

### Two Field POST Endpoints (`test_two_field_endpoints.py`)

Endpoints that accept two numeric fields. The full list is in
`two_field_endpoints_config.py`. Tests cover valid combinations, each field independently
out of range, both fields out of range, and all wrong-type/malformed scenarios.

**Request format:** `{"{FIELD1_NAME}": <float>, "{FIELD2_NAME}": <float>}`

### LED Panel - Set Single Channel (`test_setIntensity.py`)

`POST /control/led_panel/intensity/{channel}` - sets intensity for one channel (0–3).  
Tests include all valid channels, invalid channel parameters, and all input validation
categories above.

### LED Panel - Set All Channels (`test_setIntensities.py`)

`POST /control/led_panel/intensity` - sets intensities of all four channels via an array.  
**Request format:** `{"intensity": [<float>, <float>, <float>, <float>]}`  
Each element must be in `[0.0, 1.0]`.

### Pump Endpoints (`test_pump_endpoints.py`)

Exercises every endpoint for all pump instances discovered at runtime via
`GET /pumps/{instance}/pump_count` (instances 1–12). The instance→pump-count mapping is
cached and reused across tests.

| Endpoint | Method | Body |
|----------|--------|------|
| `/pumps/{i}/pump_count` | GET | — |
| `/pumps/{i}/info/{p}` | GET | — |
| `/pumps/{i}/speed/{p}` | GET / POST | `{"speed": <float>}` |
| `/pumps/{i}/flowrate/{p}` | GET / POST | `{"flowrate": <float>}` |
| `/pumps/{i}/calibration/{p}` | POST | `{"flowrate": <float>}` |
| `/pumps/{i}/move/{p}` | POST | `{"volume": <float>, "flowrate": <float>}` |
| `/pumps/{i}/stop/{p}` | GET | — |

### Calibration Endpoints (`test_calibrateSensorEndpoints.py`)

`POST /sensor/fluorometer/calibrate` and `POST /sensor/spectrophotometer/calibrate`.  
Both require an empty JSON body `{}`. Any non-empty body returns `400`.

### Print Custom Text (`test_printCustomText.py`)

`POST /sensor/oled/print_custom_text` - sends text to the OLED display.  
**Request format:** `{"text": "<string>"}`  
Tested with regular text, spaces, special characters, Unicode (Czech diacritics, emoji),
empty string, very long strings, and wrong types.

### Fluorometer Single Sample (`test_performFluorometerSingleSample.py`)

`POST /sensor/fluorometer/single_sample`  
**Valid `detector_gain` values:** `x1`, `x10`, `x50`, `Auto`  
**Valid `emitor_intensity` range:** `0.2` – `1.0`

### Fluorometer OJIP Capture (`test_captureFluorometerOjip.py`)

`POST /sensor/fluorometer/ojip/capture`

| Field | Type | Valid values |
|-------|------|-------------|
| `detector_gain` | string enum | `x1`, `x10`, `x50`, `Auto` |
| `timebase` | string enum | `linear`, `logarithmic` |
| `emitor_intensity` | float | 0.2 – 1.0 |
| `length_ms` | int | 200 – 4000 |
| `sample_count` | int | 200 – 4000 |

### Spectrophotometer Channel Info (`test_getSpectrophotometerChannelInfo.py`)

`GET /sensor/spectrophotometer/channel_info/{channel}` - valid channels: 0–5.  
Invalid channel returns `504`.

### Spectrophotometer Measure Channel (`test_measureSingleSpectrophotometerChannel.py`)

`POST /sensor/spectrophotometer/measure/{channel}` - valid channels: 0–5.  
Invalid channel returns `504`.

---

## Running Unit Tests

```bash
cd tests
source venv/bin/activate

# Run all unit tests
pytest unit/ -v

# Run a specific test module
pytest unit/test_single_field_endpoints.py -v

# Filter by test name
pytest unit/test_printCustomText.py -v
```

