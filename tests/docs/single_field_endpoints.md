# Test Scenarios: Single Field POST Endpoints

## Overview

This document describes test scenarios for POST endpoints that accept a single numeric field with min/max validation.

All endpoints share the same test structure - only the URL, field name, and valid range differ.

## Configured Endpoints

| Endpoint | URL | Field | Min | Max |
|----------|-----|-------|-----|-----|


**Note**: Configuration is in `tests/unit/single_field_endpoints_config.py`

---

## Endpoint Details

- **Request Body**: `{"{FIELD_NAME}": <float>}`
- **Valid Range**: `{MIN}` to `{MAX}` (inclusive)
- **Success Response**: 200 OK with `{"message": "{ENDPOINT_NAME} successful"}`
- **Error Response**: 400 Bad Request with `{"message": "Invalid request body: <error details>"}`

---

## Test Template

In all examples below, replace:
- `{URL}` - endpoint URL from table above
- `{FIELD_NAME}` - field name from table above  
- `{MIN}` - minimum value from table above
- `{MAX}` - maximum value from table above
- `{ENDPOINT_NAME}` - endpoint name from table above

---

## Test Categories

### 1. Valid Requests (Expected: 200)

#### Boundary Values

##### Minimum value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {MIN}}' \
  http://192.168.0.244:8089{URL}
```

##### Maximum value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {MAX}}' \
  http://192.168.0.244:8089{URL}
```

##### Zero value (if within range)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.0}' \
  http://192.168.0.244:8089{URL}
```

##### Midpoint value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": ({MIN} + {MAX}) / 2}' \
  http://192.168.0.244:8089{URL}
```

#### Floating Point Precision

##### Very small positive value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.0001}' \
  http://192.168.0.244:8089{URL}
```

##### Very small negative value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": -0.0001}' \
  http://192.168.0.244:8089{URL}
```

##### High precision value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.123456789}' \
  http://192.168.0.244:8089{URL}
```

##### Scientific notation within range
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 5.0e-1}' \
  http://192.168.0.244:8089{URL}
```

##### Integer value zero
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0}' \
  http://192.168.0.244:8089{URL}
```

##### Integer value one
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 1}' \
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

##### Null field value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": null}' \
  http://192.168.0.244:8089{URL}
```

##### Completely missing body
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089{URL}
```

#### Out of Range Values

##### Slightly below minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {MIN} - 0.0000001}' \
  http://192.168.0.244:8089{URL}
```

##### Slightly above maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {MAX} + 0.0000001}' \
  http://192.168.0.244:8089{URL}
```

##### Far below minimum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {MIN} - 100.0}' \
  http://192.168.0.244:8089{URL}
```

##### Far above maximum
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {MAX} + 100.0}' \
  http://192.168.0.244:8089{URL}
```

##### Very large positive number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 1e10}' \
  http://192.168.0.244:8089{URL}
```

##### Very large negative number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": -6.68e16}' \
  http://192.168.0.244:8089{URL}
```

#### Wrong Data Types

##### String value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": "0.5"}' \
  http://192.168.0.244:8089{URL}
```

##### Array value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": [0.5]}' \
  http://192.168.0.244:8089{URL}
```

##### Object value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": {"value": 0.5}}' \
  http://192.168.0.244:8089{URL}
```

#### Special Float Values

##### Positive Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": Infinity}' \
  http://192.168.0.244:8089{URL}
```

##### Negative Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": -Infinity}' \
  http://192.168.0.244:8089{URL}
```

##### NaN (Not a Number)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": NaN}' \
  http://192.168.0.244:8089{URL}
```

#### Malformed JSON

##### Invalid JSON syntax
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{invalid json}' \
  http://192.168.0.244:8089{URL}
```

##### Missing closing brace
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.5' \
  http://192.168.0.244:8089{URL}
```

##### Single quotes instead of double quotes
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d "{{'{FIELD_NAME}': 0.5}}" \
  http://192.168.0.244:8089{URL}
```

##### Unquoted field name
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{{FIELD_NAME}: 0.5}' \
  http://192.168.0.244:8089{URL}
```

#### HTTP Edge Cases

##### Wrong Content-Type
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{"{FIELD_NAME}": 0.5}' \
  http://192.168.0.244:8089{URL}
```

##### Empty body with Content-Type
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089{URL}
```

---

### 3. Framework Limitations (Expected: 200 OK despite invalid data)

#### Boolean to Float Conversion

##### Boolean false converted to 0.0
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": false}' \
  http://192.168.0.244:8089{URL}
```
**Reason**: Oatpp converts `false` → `0.0` before validation

##### Boolean true converted to 1.0
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": true}' \
  http://192.168.0.244:8089{URL}
```
**Reason**: Oatpp converts `true` → `1.0` before validation

#### Additional Properties

##### Additional property with simple value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.5, "extra": "value"}' \
  http://192.168.0.244:8089{URL}
```
**Reason**: Oatpp ignores additional properties not defined in DTO

##### Additional property with complex value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.5, "extra": {"nested": true}}' \
  http://192.168.0.244:8089{URL}
```
**Reason**: Oatpp ignores additional properties

##### Duplicate keys
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.0, "{FIELD_NAME}": 0.5}' \
  http://192.168.0.244:8089{URL}
```
**Reason**: Oatpp JSON parser uses last value for duplicate keys

#### Lenient JSON Parsing

##### Trailing comma accepted
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"{FIELD_NAME}": 0.5,}' \
  http://192.168.0.244:8089{URL}
```
**Reason**: Oatpp JSON parser may accept trailing commas

##### Missing Content-Type header accepted
```bash
curl -X POST \
  -d '{"{FIELD_NAME}": 0.5}' \
  http://192.168.0.244:8089{URL}
``` 
**Reason**: Oatpp assumes JSON by default

---


## Test Implementation

Tests are implemented in `tests/unit/test_single_field_endpoints.py` using pytest parametrization.

**Run all tests:**
```bash
cd tests
pytest unit/test_single_field_endpoints.py -v
```

**Run tests for specific endpoint:**
```bash
pytest unit/test_single_field_endpoints.py -k "setHeaterIntensity" -v
```

**Add new endpoint:**
Edit `tests/unit/single_fieldendpoints_config.py` and add entry to `SINGLE_FIELD_ENDPOINTS` list.


