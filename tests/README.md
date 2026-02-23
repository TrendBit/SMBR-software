# Python API Testing

This directory contains all automated tests for the SMBR REST API.  The primary framework is
**pytest**; a subset of endpoints is also exercised via **Schemathesis** for schema‑based
generated inputs.

## Preparing a Python environment

A virtual environment is recommended to avoid polluting the system Python.  Example using
`venv`:

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

Once dependencies are installed and the virtualenv activated, you can either run `pytest`
directly or use the provided helper script.

### One‑shot script

A convenience shell script `run_tests.sh` lives in this directory.  When executed it will:

1. run `pytest -v` against the entire `tests` tree
2. read `BASE_URL` from `config.py` and echo it
3. collect operation ids from `schemathesis/operations.txt`
4. invoke Schemathesis against the server's OpenAPI spec

Execute it like so:

```bash
cd tests
./run_tests.sh
```

### Pytest directly

Use `pytest` from the `tests` directory. 

### Run everything
```bash
pytest -v
```

### Filter by name
```bash
pytest -k printCustomText    
```

### Stop on first failure
```bash
pytest -x
```

---

## Schemathesis fuzzing

The `tests/schemathesis/` directory contains an OpenAPI schema and operation list used by
Schemathesis to generate randomized requests. Running these tests helps catch edge cases
uncovered by the hand‑written pytest modules.

To execute the Schemathesis suite:

```bash
cd tests
schemathesis run http://<device>:<port>/api-docs/oas-3.0.0.json --include-operation-id <operation>
```

The `operations.txt` file lists the operations to include.
## Project Structure

- **docs/**: documentation for each test category (single‑field, two‑field, pump, etc.)
- **unit/**: pytest scripts and configuration for API tests
- **schemathesis/**: schema/operation configuration for fuzz testing
- `config.py`: host/port configuration
- `requirements.txt`: Python dependencies

For detailed scenarios see the docs directory:

```bash
ls docs/*.md
```



