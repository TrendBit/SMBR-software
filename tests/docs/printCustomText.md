# POST /sensor/oled/print_custom_text - Test Scenarios

Endpoint for printing custom text on Mini OLED display.

**Valid format:**
```json
{"text": "Hello World"}
```

---

## Valid Requests (Expected: 200)

### Regular text
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Hello World"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Empty string (valid)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": ""}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Text with spaces
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "   text with spaces   "}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Spaces only
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "   "}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Special characters
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Test123!@#$%"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Extended special characters
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Special: @#$%^&*()"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Unicode characters (Czech diacritics)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Příliš žluťoučký kůň"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### International Unicode (Chinese, emoji)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Unicode: 你好世界 🌍"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Text with newlines (escaped)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Line1\nLine2"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Text with multiple newlines
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "Line1\nLine2\nLine3"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Long text
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "This is a very long text that might overflow the display but should still be accepted by the API"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Very long text (1000 chars)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

---

## Errors Caught by ErrorHandler (Expected: 400)

### Empty body
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Missing text field
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"message": "hello"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Null instead of string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": null}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Number instead of string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": 123}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Boolean instead of string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": true}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Array instead of string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": ["hello", "world"]}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Object instead of string
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": {"message": "hello"}}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Invalid JSON (missing quotes)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{text: hello}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Invalid JSON (unclosed bracket)
```bash
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "hello"' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```


---

## Framework Limitations (Expected: 200 - accepted due to oatpp)

### Additional property with simple value
```bash
# oatpp ignores all additional properties (simple and complex)
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "hello", "extra": "value"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```
### Additional property with complex value
```bash
# oatpp also ignores object additional properties
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "hello", "extra": {"nested": true}}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```
### Duplicate keys (last value used)
```bash
# JSON RFC allows, last value is used
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "first", "text": "second"}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

### Trailing comma (accepted by oatpp parser)
```bash
# oatpp parser is lenient and accepts trailing commas
curl -X POST -H 'Content-Type: application/json' \
  -d '{"text": "hello",}' \
  http://192.168.0.244:8089/sensor/oled/print_custom_text
```

