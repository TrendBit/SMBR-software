# Test Scenarios: Pump‑related Endpoints

## Overview

The controller exposes a set of endpoints for querying and controlling individual pumps.  The `tests/unit/test_pump_endpoints.py` module exercises every endpoint against **all of the pump instances that respond**.  Discovery is performed once at the beginning of the module (instances are numbered 1–12) and the resulting instance→pump‑count mapping is reused throughout the rail of tests.

## Configured Endpoints

| Action | URL pattern | Method | Notes |
|--------|-------------|--------|-------|
| pump count | `/pumps/{instance}/pump_count` | GET | returns `{"pump_count": <int>}` |
| pump info  | `/pumps/{instance}/info/{pump}` | GET | returns `{"max_flowrate": <float>, "min_flowrate": <float>}` |
| get speed  | `/pumps/{instance}/speed/{pump}` | GET | returns `{"speed": <float>}` |
| set speed  | `/pumps/{instance}/speed/{pump}` | POST | body `{"speed": <float>}` |
| get flowrate | `/pumps/{instance}/flowrate/{pump}` | GET | returns `{"flowrate": <float>}` |
| set flowrate | `/pumps/{instance}/flowrate/{pump}` | POST | body `{"flowrate": <float>}` |
| calibrate    | `/pumps/{instance}/calibration/{pump}` | POST | body `{"flowrate": <float>}` |
| move         | `/pumps/{instance}/move/{pump}` | POST | body `{"volume": <float>, "flowrate": <float>}` |
| stop         | `/pumps/{instance}/stop/{pump}` | GET |

## Detailed Scenarios

### Pump count (`GET /pumps/{instance}/pump_count`)

- **Valid**: any discovered instance index returns 200 and a JSON object containing `"pump_count": <integer>`.  The mapping itself is cached in the fixture.
- **Invalid**: instance 0 or 13 (or any index outside 1..12) returns 400.

### Pump info (`GET /pumps/{instance}/info/{pump}`)

- **Valid**: pump index between 1 and the value returned by `/pump_count`.
- **Invalid**: pump 0 or `count+1` should yield 400.

### Numeric control endpoints (speed, flowrate, calibration)

All three endpoints use the same validation logic; only the path and allowed range change. Below are the categories of requests exercised by the tests:

1. **Valid payloads**
   * minimum value
   * maximum value
   * zero (if within range)
   * midpoint
   * high‑precision floats, scientific notation, integer values
2. **Bodies that trigger 400**
   * empty JSON (`{}`) or missing body entirely
   * `null` value
   * value just outside the permitted range
   * value far outside range
   * very large positive/negative numbers
   * boolean `true`/`false` (framework may convert)
   * wrong data types (string, array, object)
   * malformed JSON (missing brace, unquoted key)
   * special numbers (`Infinity`, `-Infinity`, `NaN`)
   * incorrect `Content-Type` header

### Move endpoint (`POST /pumps/{instance}/move/{pump}`)

Two‑field body with `volume` and `flowrate`.  Tests cover:

- valid zero payload
- missing field1, missing field2, both missing
- `null` in one or both fields
- out‑of‑range for either field separately
- type errors on either field
- malformed JSON variations

Ranges used in examples:
- `volume`: -1000.0–1000.0
- `flowrate`: 0.0–1000.0

### Stop endpoint (`GET /pumps/{instance}/stop/{pump}`)

Simple GET; valid for every pump, invalid for pump 0 or `count+1`.

