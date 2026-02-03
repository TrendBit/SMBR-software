# Test Scenarios: Two Field POST Endpoints

## Overview

This document describes test scenarios for POST endpoints that accept two numeric fields with min/max validation.

All endpoints share the same test structure - only the URL, field names, and valid ranges differ.

## Configured Endpoints

| Endpoint | URL | Field 1 | Min | Max | Field 2 | Min | Max |
|----------|-----|---------|-----|-----|---------|-----|-----|
| moveCuvettePump | `/control/cuvette_pump/move` | `volume` | 0.0 | 1000.0 | `flowrate` | -1000.0 | 1000.0 |

**Note**: Configuration is in `tests/unit/two_field_endpoints_config.py`

---

## Endpoint Details

- **Request Body**: `{"{FIELD1_NAME}": <float>, "{FIELD2_NAME}": <float>}`
- **Valid Ranges**: 
  - Field 1: `{FIELD1_MIN}` to `{FIELD1_MAX}` (inclusive)
  - Field 2: `{FIELD2_MIN}` to `{FIELD2_MAX}` (inclusive)
- **Success Response**: 200 OK with `{"message": "{ENDPOINT_NAME} successful"}`
- **Error Response**: 400 Bad Request with `{"message": "Invalid request body: <error details>"}`

---

## Test Template

In all examples below, replace:
- `{URL}` - endpoint URL from table above
- `{FIELD1_NAME}` - first field name from table above  
- `{FIELD1_MIN}`, `{FIELD1_MAX}` - min/max for field 1 from table above
- `{FIELD2_NAME}` - second field name from table above  
- `{FIELD2_MIN}`, `{FIELD2_MAX}` - min/max for field 2 from table above
- `{ENDPOINT_NAME}` - endpoint name from table above

---

## Test Categories

### 1. Valid Requests (Expected: 200)

#### Boundary Values

##### Both fields at minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MIN}, "{FIELD2_NAME}": {FIELD2_MIN}}' \
  http://192.168.0.244:8089{URL}
```

##### Both fields at maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MAX}, "{FIELD2_NAME}": {FIELD2_MAX}}' \
  http://192.168.0.244:8089{URL}
```

##### Both fields at zero (if within range)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 0.0, "{FIELD2_NAME}": 0.0}' \
  http://192.168.0.244:8089{URL}
```

##### Both fields at midpoint
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": ({FIELD1_MIN} + {FIELD1_MAX}) / 2, "{FIELD2_NAME}": ({FIELD2_MIN} + {FIELD2_MAX}) / 2}' \
  http://192.168.0.244:8089{URL}
```

#### Mixed Boundary Values

##### Field1 minimum, Field2 maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MIN}, "{FIELD2_NAME}": {FIELD2_MAX}}' \
  http://192.168.0.244:8089{URL}
```

##### Field1 maximum, Field2 minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MAX}, "{FIELD2_NAME}": {FIELD2_MIN}}' \
  http://192.168.0.244:8089{URL}
```

#### Floating Point Precision

##### High precision values
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 123.456789123, "{FIELD2_NAME}": 987.654321987}' \
  http://192.168.0.244:8089{URL}
```

##### Scientific notation within range
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 5.0e1, "{FIELD2_NAME}": 1.5e2}' \
  http://192.168.0.244:8089{URL}
```

#### Integer Values

##### Integer values (converted to float)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100, "{FIELD2_NAME}": 200}' \
  http://192.168.0.244:8089{URL}
```

##### Mixed integer and float
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100, "{FIELD2_NAME}": 200.5}' \
  http://192.168.0.244:8089{URL}
```

---

### 2. Error Cases (Expected: 400)

#### Missing or Invalid Request Body

##### Empty request body
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089{URL}
```

##### Missing field1
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD2_NAME}": 100.0}' \
  http://192.168.0.244:8089{URL}
```

##### Missing field2
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0}' \
  http://192.168.0.244:8089{URL}
```

##### Null field1 value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": null, "{FIELD2_NAME}": 100.0}' \
  http://192.168.0.244:8089{URL}
```

##### Null field2 value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": null}' \
  http://192.168.0.244:8089{URL}
```

##### Both fields null
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": null, "{FIELD2_NAME}": null}' \
  http://192.168.0.244:8089{URL}
```

##### Completely missing body
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089{URL}
```

#### Out of Range Values - Field 1

##### Field1 slightly below minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MIN} - 0.001, "{FIELD2_NAME}": ({FIELD2_MIN} + {FIELD2_MAX}) / 2}' \
  http://192.168.0.244:8089{URL}
```

##### Field1 above maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MAX} + 0.001, "{FIELD2_NAME}": ({FIELD2_MIN} + {FIELD2_MAX}) / 2}' \
  http://192.168.0.244:8089{URL}
```

#### Out of Range Values - Field 2

##### Field2 below minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": ({FIELD1_MIN} + {FIELD1_MAX}) / 2, "{FIELD2_NAME}": {FIELD2_MIN} - 0.001}' \
  http://192.168.0.244:8089{URL}
```

##### Field2 above maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": ({FIELD1_MIN} + {FIELD1_MAX}) / 2, "{FIELD2_NAME}": {FIELD2_MAX} + 0.001}' \
  http://192.168.0.244:8089{URL}
```

#### Out of Range Values - Both Fields

##### Both fields below minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MIN} - 1.0, "{FIELD2_NAME}": {FIELD2_MIN} - 1.0}' \
  http://192.168.0.244:8089{URL}
```

##### Both fields above maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": {FIELD1_MAX} + 1.0, "{FIELD2_NAME}": {FIELD2_MAX} + 1.0}' \
  http://192.168.0.244:8089{URL}
```

#### Wrong Data Types - Field 1

##### Field1 as string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": "100.5", "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```

##### Field1 as array
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": [100.5], "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```

#### Wrong Data Types - Field 2

##### Field2 as string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": "200.5"}' \
  http://192.168.0.244:8089{URL}
```

##### Field2 as object
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": {"value": 200.5}}' \
  http://192.168.0.244:8089{URL}
```

#### Wrong Data Types - Both Fields

##### Both fields as strings
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": "100.5", "{FIELD2_NAME}": "200.5"}' \
  http://192.168.0.244:8089{URL}
```

#### Very Large Values

##### Very large field1
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 1e10, "{FIELD2_NAME}": ({FIELD2_MIN} + {FIELD2_MAX}) / 2}' \
  http://192.168.0.244:8089{URL}
```

##### Very large field2
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": ({FIELD1_MIN} + {FIELD1_MAX}) / 2, "{FIELD2_NAME}": 1e10}' \
  http://192.168.0.244:8089{URL}
```

#### Malformed JSON

##### Missing closing brace
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": 200.0' \
  http://192.168.0.244:8089{URL}
```

##### Single quotes instead of double quotes
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d "{{'{FIELD1_NAME}': 100.0, '{FIELD2_NAME}': 200.0}}" \
  http://192.168.0.244:8089{URL}
```

##### Unquoted field names
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{{{FIELD1_NAME}: 100.0, {FIELD2_NAME}: 200.0}}' \
  http://192.168.0.244:8089{URL}
```

##### Completely invalid JSON
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{invalid json}' \
  http://192.168.0.244:8089{URL}
```

#### Special Float Values

##### Field1 as Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": Infinity, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```

##### Field1 as negative Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": -Infinity, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```

##### Field1 as NaN
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": NaN, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```

##### Field2 as Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": Infinity}' \
  http://192.168.0.244:8089{URL}
```

##### Field2 as negative Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": -Infinity}' \
  http://192.168.0.244:8089{URL}
```

##### Field2 as NaN
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": NaN}' \
  http://192.168.0.244:8089{URL}
```

#### HTTP Edge Cases

##### Wrong Content-Type
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```

---

### 3. Framework Limitations (200 OK despite invalid data)

#### Additional Properties

##### Extra field (framework ignores it)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": 200.0, "extra": "value"}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK - Framework accepts additional properties

#### Boolean Values

##### Field1 as boolean false (converted to 0.0)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": false, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK if 0.0 is within field1 range, otherwise 400

##### Field1 as boolean true (converted to 1.0)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": true, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK if 1.0 is within field1 range, otherwise 400

##### Field2 as boolean false (converted to 0.0)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": false}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK if 0.0 is within field2 range, otherwise 400

##### Missing comma between fields (framework accepts it)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0 "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK - Framework parses JSON without comma

#### Duplicate Keys

##### Duplicate field keys (framework uses last value)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 0.0, "{FIELD2_NAME}": 100.0, "{FIELD1_NAME}": 500.0}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK - Framework uses last value (500.0) for {FIELD1_NAME}

#### Lenient JSON Parsing

##### Trailing comma accepted
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": 200.0,}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK - Framework accepts trailing comma

##### Missing Content-Type header
```bash
curl -X POST \
  -d '{"{FIELD1_NAME}": 100.0, "{FIELD2_NAME}": 200.0}' \
  http://192.168.0.244:8089{URL}
```
**Result**: 200 OK - Framework assumes JSON by default

---


## Test Implementation

Tests are implemented in `tests/unit/test_two_field_endpoints.py` using pytest parametrization.

**Run all tests:**
```bash
cd tests
pytest unit/test_two_field_endpoints.py -v
```

**Run tests for specific endpoint:**
```bash
pytest unit/test_two_field_endpoints.py -k "moveCuvettePump" -v
```

**Add new endpoint:**
Edit `tests/unit/two_fieldendpoints_config.py` and add entry to `TWO_FIELD_ENDPOINTS` list.

