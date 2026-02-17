# POST /sensor/fluorometer/ojip/capture - Test Scenarios

Endpoint for capturing fluorometer OJIP measurement data.

**Valid detector_gain values:** `x1`, `x10`, `x50`, `Auto`  
**Valid timebase values:** `linear`, `logarithmic`  
**Valid emitor_intensity range:** `0.2` to `1.0`  
**Valid length_ms range:** `200` to `4000`  
**Valid sample_count range:** `200` to `4000`

**Valid format:**
```json
{
  "detector_gain": "x1",
  "timebase": "logarithmic",
  "emitor_intensity": 1,
  "length_ms": 1000,
  "sample_count": 1000
}
```

---

## Valid Requests (Expected: 200)

### Valid detector_gain + timebase combinations
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"Auto","timebase":"logarithmic","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Valid boundaries for length_ms
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":4000,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Valid boundaries for sample_count
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":4000}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Valid boundaries for emitor_intensity
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.2,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":1.0,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

---

## Errors Caught by Validation/ErrorHandler (Expected: 400)

### Missing required parameters
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1"}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Null required parameters
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":null,"timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Invalid ranges
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":199,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":4001}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":1.0001,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Invalid enums
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x2","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"log","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Wrong types
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":1,"timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":"0.5","length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Malformed JSON
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{detector_gain:"x1",timebase:"linear",emitor_intensity:0.5,length_ms:200,sample_count:200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

### Missing request body
```bash
curl -X POST -H 'Content-Type: application/json' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```

## Wrong Content-Type 
```bash
curl -X POST -H 'Content-Type: text/plain' \
  -d '{"detector_gain":"x1","timebase":"linear","emitor_intensity":0.5,"length_ms":200,"sample_count":200}' \
  http://192.168.0.244:8089/sensor/fluorometer/ojip/capture
```
