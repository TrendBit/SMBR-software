# GET /sensor/spectrophotometer/channel_info/{channel} - Test Scenarios

Endpoint for reading spectrophotometer channel information.

**Valid channels:** 0, 1, 2, 3, 4, 5

---

## Valid Requests (Expected: 200)

### Channel 0
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/0
```

### Channel 1
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/1
```

### Channel 2
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/2
```

### Channel 3
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/3
```

### Channel 4
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/4
```

### Channel 5
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/5
```

---

## Invalid Channel Parameters (Expected: 504)

### Invalid channel - negative
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/-1
```

### Invalid channel - too high
```bash
curl -X GET \
  http://192.168.0.244:8089/sensor/spectrophotometer/channel_info/7
```

