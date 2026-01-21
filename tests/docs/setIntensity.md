# POST /control/led_panel/intensity/{channel} - Test Scenarios

Endpoint for setting intensity of a specific LED panel channel.

**Valid channels:** 0, 1, 2, 3

**Valid format:**
```json
{"intensity": 0.5}
```

---

## Valid Requests (Expected: 200)

### Channel 0 - minimum intensity (0.0)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.0}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Channel 0 - maximum intensity (1.0)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 1.0}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Channel 0 - middle intensity (0.5)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Channel 0 - decimal value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.123456}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Channel 1 - intensity 0.75
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.75}' \
  http://192.168.0.244:8089/control/led_panel/intensity/1
```

### Channel 2 - intensity 0.25
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.25}' \
  http://192.168.0.244:8089/control/led_panel/intensity/2
```

### Channel 3 - intensity 0.999
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.999}' \
  http://192.168.0.244:8089/control/led_panel/intensity/3
```

### Integer value 0 (converted to float)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Integer value 1 (converted to float)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 1}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Very small positive number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.0001}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Number very close to 1
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.9999999}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Very long decimal precision
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.123456789012345678}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

---

## Errors Caught by Code Validation or ErrorHandler (Expected: 400)

### Empty body
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Missing intensity field
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"other": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Null intensity value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": null}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Value out of range - negative
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": -0.1}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Value out of range - greater than 1
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 1.1}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Large negative value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": -100}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Large positive value
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 1000}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Very large number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 5.761053081236682e+16}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Very small negative number (boundary)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": -0.0000001}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Just above 1.0 (boundary)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 1.0000001}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Special float - Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": Infinity}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Special float - Negative Infinity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": -Infinity}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Special float - NaN
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": NaN}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### String instead of number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": "text"}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Array instead of number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": [0.5]}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Object instead of number
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": {"value": 0.5}}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Invalid JSON
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{invalid json}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Malformed JSON - missing quotes
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{intensity: 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Wrong Content-Type header
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Empty request body (no JSON at all)
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

---

## Invalid Channel Parameters (Expected: 400)

### Invalid channel - negative
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/-1
```

### Invalid channel - too high
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/4
```

### Invalid channel - text
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/channel0
```

### Invalid channel - float
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/1.5
```

### Invalid channel - empty
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5}' \
  http://192.168.0.244:8089/control/led_panel/intensity/
```

---

## Framework Limitations (Expected: 200 - accepted due to oatpp)

### Boolean false converted to 0.0
```bash
# oatpp converts false->0.0
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": false}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Boolean true converted to 1.0
```bash
# oatpp converts true->1.0
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": true}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Additional property with simple value
```bash
# oatpp ignores additional properties
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5, "extra": "value"}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Additional property with complex value
```bash
# oatpp also accepts complex additional properties
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5, "extra": {"nested": true}}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Duplicate keys (last value used)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.0, "intensity": 1.0}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

### Trailing comma accepted
```bash
# JSON parser tolerates trailing commas
curl -X POST -H 'Content-Type: application/json' \
  -d '{"intensity": 0.5,}' \
  http://192.168.0.244:8089/control/led_panel/intensity/0
```

