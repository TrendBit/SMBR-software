# Schemathesis Tests Documentation

## Overview

Schemathesis tests verify the API by generating randomised and schema-derived requests
directly from the server's OpenAPI specification. They complement the hand-written unit
tests by automatically exercising edge cases, unexpected input combinations, and schema
compliance.

Tests are configured in `tests/schemathesis/` and driven by the `schemathesis.toml` file.

---

## How Schemathesis Works

1. Schemathesis fetches the live OpenAPI spec from the running server.
2. For each included operation it generates a large number of random request payloads.
3. It checks each response for:
   - **Status code conformance** - the returned status code must be declared in the spec.
   - **Response schema conformance** - the response body must match the declared schema.
   - **Unsupported method** - the server must not accept HTTP methods not listed in the spec.

---

## Covered Operations

All operations listed in `schemathesis/operations.txt` are included in the test run.
The current list:

**System / diagnostics**
- `getSystemModules`, `getSystemErrors`, `getSystemWarnings`, `getModuleIssues`
- `getCanRxPackets`, `getCanTxPackets`, `getCanRxErrors`, `getCanTxErrors`
- `getCanRxDropped`, `getCanTxDropped`, `getCanCollisions`
- `ping`, `getCoreLoad`, `getCoreTemp`, `getBoardTemp`, `getFwVersion`, `getHwVersion`

**Device identity**
- `getShortID`, `getIpAddress`, `getHostname`, `getSerialNumber`, `getModel`
- `getPowerSupplyType`, `getVoltage5V`, `getVoltageVIN`, `getPoEVoltage`
- `getCurrentConsumption`, `getPowerDraw`

**LED panel**
- `getIntensity`, `getLedTemperature`

**Heater**
- `getHeaterIntensity`, `getHeaterTargetTemperature`, `getHeaterPlateTemperature`
- `turnOffHeater`

**Cuvette pump**
- `getCuvettePumpInfo`, `getCuvettePumpSpeed`, `getCuvettePumpFlowrate`, `stopCuvettePump`

**Aerator**
- `getAeratorInfo`, `getAeratorSpeed`, `getAeratorFlowrate`, `stopAerator`

**Mixer**
- `getMixerInfo`, `getMixerSpeed`, `getMixerRpm`, `stopMixer`

---

## Configuration (`schemathesis.toml`)

Key settings in `tests/schemathesis.toml`:

| Setting | Value | Reason |
|---------|-------|--------|
| `checks.unsupported_method` | disabled | Not relevant for this server |
| `checks.response_schema_conformance` (heater target_temp GET) | disabled | Known deviation |
| `checks.status_code_conformance` (ping/load/… GET) | disabled | Known deviation |

**Fixed path-parameter values** (to avoid invalid hardware-level requests):

| Pattern | Allowed values |
|---------|---------------|
| `GET /{module}/ping` | `core`, `control`, `sensor` |
| `GET /{module}/load` | `core`, `control`, `sensor` |
| `GET /{module}/core_temp` | `core`, `control`, `sensor` |
| `GET /{module}/board_temp` | `core`, `control`, `sensor` |
| `GET /{module}/fw_version` | `core`, `control`, `sensor` |
| `GET /{module}/hw_version` | `core`, `control`, `sensor` |
| `GET /control/led_panel/intensity/{channel}` | `0`, `1`, `2`, `3` |

---

## Running Schemathesis Tests

### Prerequisites

Virtual environment with dependencies installed:

```bash
cd tests
source venv/bin/activate
```

### Run all configured operations (via helper script)

The dedicated `schemathesis/run_schemathesis.sh` script reads operation IDs from
`schemathesis/operations.txt` and invokes Schemathesis for each:

```bash
cd tests
./schemathesis/run_schemathesis.sh
```

### Run a single operation

```bash
schemathesis run http://<device>:<port>/api-docs/oas-3.0.0.json \
  --include-operation-id <operationId>
```

### Example - run getSystemModules

```bash
schemathesis run http://192.168.0.244:8089/api-docs/oas-3.0.0.json \
  --include-operation-id getSystemModules
```

