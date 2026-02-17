# POST /sensor/spectrophotometer/measure/{channel} - Test Scenarios

Endpoint for measuring a selected spectrophotometer channel.

**Valid channels:** 0, 1, 2, 3, 4, 5

---

## Valid Requests (Expected: 200)

### Measure channel 0
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/0
```

### Measure channel 1
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/1
```

### Measure channel 2
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/2
```

### Measure channel 3
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/3
```

### Measure channel 4
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/4
```

### Measure channel 5
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/5
```
---

## Invalid Channel Parameters

### Invalid channel - negative (Expected: 504)
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/-1
```

### Invalid channel - too high (Expected: 504)
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/7
```

### Invalid channel - text (Expected: 400)
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/abc
```

### Invalid channel - decimal format (Expected: 400)
```bash
curl -X POST \
  http://192.168.0.244:8089/sensor/spectrophotometer/measure/1.5
```


