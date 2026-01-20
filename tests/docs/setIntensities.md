# POST /control/led_panel/intensity - Test Scenarios

Endpoint for setting intensities of all LED panel channels.

**Valid format:**
```json
{"intensity": [0.5, 1.0, 0.0, 0.2]}
```

---

## Valid Requests (Expected: 200)

### Correct format
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, 1, 0, 0.2]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### All zeros
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0, 0, 0, 0]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### All ones
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [1.0, 1.0, 1.0, 1.0]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Decimal values
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.123, 0.456, 0.789, 0.999]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

---

## Errors Caught by ErrorHandler (Expected: 400)

### Empty body
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Missing intensity field
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"other": [1,2,3,4]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Null instead of array
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": null}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### String instead of array
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": "text"}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Object instead of array
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": {"a": 1}}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Number instead of array
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 123}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Wrong type in array - string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, "text", 0, 0.2]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Wrong type in array - object
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, {"a":1}, 0, 0.2]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Invalid JSON
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{invalid json}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Empty array
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": []}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Only 1 element
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Only 2 elements
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, 1]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Only 3 elements
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, 1, 0]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### 5 elements
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, 1, 0, 0.2, 0.3]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Null element in array (correct size, but null value)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [null, 1, 0, 0.2]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### All nulls
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [null, null, null, null]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Value out of range - negative
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [-0.1, 1, 0, 0.2]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Value out of range - greater than 1
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5, 1.1, 0, 0.2]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### All values out of range
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [-1, 2, -0.5, 1.5]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

---

## Framework Limitations (Expected: 200 - accepted due to oatpp)

### Additional property with simple value
```bash
# oatpp ignores simple additional properties
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5,1,0,0.2], "extra": "value"}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Additional property with complex value
```bash
# oatpp also accepts complex additional properties
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5,1,0,0.2], "extra": {"nested": true}}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Duplicate keys (last value used)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0,0,0,0], "intensity": [1,1,1,1]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```

### Boolean values converted to numbers
```bash
# oatpp converts false->0 and true->1
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [false, true, true, false]}' \
  http://192.168.0.244:8089/control/led_panel/intensity
```
