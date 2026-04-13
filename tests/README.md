# API Testing

This directory contains all automated tests for the SMBR REST API. Three types of testing
are used:

| Type | Location | Tool | Documentation |
|------|----------|------|---------------|
| **Unit** | `unit/` | pytest | `docs/unit.md` |
| **Behavioral** | `behavioral/` | pytest | `docs/behavioral.md` |
| **Schemathesis** | `schemathesis/` | schemathesis | `docs/schemathesis.md` |

## Preparing a Python Environment

A virtual environment is recommended to avoid polluting the system Python:

```bash
cd tests
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

Any subsequent invocation of `pytest` or `schemathesis` should be done with the venv activated.

## Configuration

Edit device IP and port in `config.py`:

```python
DEVICE_IP = "192.168.0.244"
DEVICE_PORT = 8089
```

## Running Tests

### All tests at once

`run_tests.sh` runs all three suites sequentially and prints a final summary:

```bash
cd tests
./run_tests.sh
```

Each suite runs to completion regardless of whether a previous one failed. The script exits
with a non-zero code if any suite failed.

### Unit tests only

```bash
cd tests
pytest unit/ -v
```

### Behavioral tests only

```bash
cd tests
pytest behavioral/ -v
```

### Schemathesis only

```bash
cd tests
./schemathesis/run_schemathesis.sh
```

---

## Project Structure

```
tests/
├── run_tests.sh                  # Run all suites (unit + behavioral + schemathesis)
├── config.py                     # Device IP and port
├── requirements.txt              # Python dependencies
├── conftest.py
├── docs/
│   ├── unit.md                   # Unit test documentation
│   ├── behavioral.md             # Behavioral test documentation
│   └── schemathesis.md           # Schemathesis documentation
├── unit/                         # Unit tests (input validation)
├── behavioral/                   # Behavioral tests (end-to-end system behaviour)
└── schemathesis/
    ├── run_schemathesis.sh       # Run schemathesis suite only
    └── operations.txt            # Operation IDs to include
```



