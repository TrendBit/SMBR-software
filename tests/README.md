# Python API Testing

Automated testing for SMBR API using pytest.

## Installation

```bash
cd tests
pip install -r requirements.txt
```

## Configuration

Edit device IP and port in `config.py`:

```python
DEVICE_IP = "192.168.0.244"
DEVICE_PORT = 8089
```

## Running Tests

Pytest automatically discovers all `test_*.py` files

### All tests
```bash
pytest
```

### Specific folder
```bash
pytest unit/             # Run unit tests only
```

### Filter by name
```bash
pytest -k printCustomText    # Run tests matching "printCustomText"
```

### Verbose output
```bash
pytest -v               # Verbose
pytest -vv              # Very verbose
```

### Stop on first failure
```bash
pytest -x
```

## Project Structure

- **Documentation**: See `docs/` for detailed endpoint test scenarios
- **Test scripts**: See `unit/` folder - pytest finds them automatically



