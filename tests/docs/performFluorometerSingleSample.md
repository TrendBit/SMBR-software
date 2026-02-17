# POST /sensor/fluorometer/single_sample - Test Scenarios

Endpoint for performing a single fluorometer sample measurement.

**Valid detector_gain values:** `x1`, `x10`, `x50`, `Auto`  
**Valid emitor_intensity range:** `0.2` to `1.0` 

**Valid format:**
```json
{"detector_gain": "x1", "emitor_intensity": 0.5}
```

---

## Valid Requests (Expected: 200)

### Valid detector_gain values
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x10", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x50", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "Auto", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Valid emitor_intensity boundaries
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.2}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 1.0}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Valid detector_gain + emitor_intensity combinations
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.2}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x10", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x50", "emitor_intensity": 1.0}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "Auto", "emitor_intensity": 0.2}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "Auto", "emitor_intensity": 1.0}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

---

## Errors Caught by Validation/ErrorHandler (Expected: 400)

### emitor_intensity outside valid range
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.1999}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 1.0001}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": -1.0}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 2.0}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Invalid detector_gain values
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x2", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "auto", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "AUTO", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x100", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Missing/null required fields
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1"}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": null, "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": null}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Invalid JSON
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.5' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{detector_gain: "x1", emitor_intensity: 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Missing request body
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Wrong Content-Type
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Wrong type for detector_gain
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": 1, "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": true, "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": ["x1"], "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": {"gain": "x1"}, "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Wrong type for emitor_intensity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": "0.5"}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": [0.5]}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": {"value": 0.5}}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Special float values for emitor_intensity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": Infinity}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": -Infinity}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": NaN}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

---

## Framework Limitations (Expected: 200)

### Additional property (simple)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.5, "extra": "value"}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Additional property (complex)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.5, "extra": {"nested": true}}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Duplicate keys (last value used)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.4, "emitor_intensity": 0.5}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```

### Trailing comma accepted by parser
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain": "x1", "emitor_intensity": 0.5,}' \
  http://192.168.0.244:8089/sensor/fluorometer/single_sample
```


