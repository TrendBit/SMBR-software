# POST Calibration Endpoints - Test Scenarios

Endpoints covered:
- `/sensor/fluorometer/calibrate`
- `/sensor/spectrophotometer/calibrate`

Both endpoints require **exactly** empty JSON body:
```json
{}
```

---

## Valid Requests (Expected: 200)

### Fluorometer calibration with empty JSON body
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```

### Spectrophotometer calibration with empty JSON body
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

### Framework behavior: text/plain with empty object still accepted
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

---

## Error Cases (Expected: 400)

### Missing request body
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

### Non-empty JSON object
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"calibrationMode": "default"}' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"extra": 1}' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

### Invalid JSON structure (array/string/null)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '[]' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '"text"' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d 'null' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```

### Malformed JSON
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"calibrationMode": "default"' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{calibrationMode: default}' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```

### Whitespace-only variant of empty object (not exactly {})
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{ }' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{ }' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

### Duplicate keys payload
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"a": 1, "a": 2}' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"a": 1, "a": 2}' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

### Trailing comma in object
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"calibrationMode": null,}' \
  http://192.168.0.244:8089/sensor/fluorometer/calibrate
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"calibrationMode": null,}' \
  http://192.168.0.244:8089/sensor/spectrophotometer/calibrate
```

