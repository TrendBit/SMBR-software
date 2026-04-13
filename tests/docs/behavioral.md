# Behavioral Tests Documentation

## Overview

Behavioral tests verify that actions performed via one endpoint are correctly reflected in
state readable via another endpoint. Unlike unit tests (which focus on input validation),
these tests exercise end-to-end system behaviour: actuation, state persistence, physical
effects, and concurrency.

Tests are implemented using **pytest** and are located in `tests/behavioral/`. All test
parameters (values, tolerances, timeouts, URLs) are defined in the corresponding
`*_config.py`.

---

## Test Modules

| Test file | Config file | Category |
|-----------|-------------|----------|
| `test_set_get.py` | `set_get_config.py` | Set–Get symmetry |
| `test_stop_zero.py` | `stop_zero_config.py` | Stop → zero output |
| `test_power_draw.py` | `power_draw_config.py` | Physical effects (power) |
| `test_thermal_effects.py` | `thermal_effects_config.py` | Thermal effects |
| `test_concurrency.py` | `concurrency_config.py` | Concurrency & race conditions |
| `test_state_persistence.py` | `state_persistence_config.py` | State persistence after restart |
| `test_fluorometer_ojip.py` | `fluorometer_ojip_config.py` | Fluorometer OJIP workflow |
| `test_limits.py` | `limits_config.py` | Limits from info endpoints |
| `test_move.py` | `move_config.py` | Move → completion / interruption |
| `test_system_stats.py` | `system_stats_config.py` | System statistics |

---

## Category 1 - Set–Get Symmetry

**Config:** `set_get_config.py` | **Test:** `test_set_get.py`

Set a value via POST, read it back via GET - the values must match within tolerance.

### Heater

| POST | Value(s) | GET | Tolerance |
|------|----------|-----|-----------|
| `POST /control/heater/target_temperature` | 20.0, 37.0, 50.0 °C | `GET /control/heater/target_temperature` | ±0.01 |
| `POST /control/heater/intensity` | -1.0, 0.0, 1.0 | `GET /control/heater/intensity` | ±0.01 |

### Cuvette pump

| POST | Value(s) | GET | Tolerance |
|------|----------|-----|-----------|
| `POST /control/cuvette_pump/speed` | -1.0, 0.0, 1.0 | `GET /control/cuvette_pump/speed` | ±0.01 |
| `POST /control/cuvette_pump/flowrate` | -60.0, 100.0 ml/min | `GET /control/cuvette_pump/flowrate` | ±0.1 |

### Aerator

| POST | Value(s) | GET | Tolerance |
|------|----------|-----|-----------|
| `POST /control/aerator/speed` | 0.0, 0.5, 1.0 | `GET /control/aerator/speed` | ±0.01 |
| `POST /control/aerator/flowrate` | 200.0, 2000.0 ml/min | `GET /control/aerator/flowrate` | ±1.0 |

### Mixer

| POST | Value(s) | GET | Tolerance |
|------|----------|-----|-----------|
| `POST /control/mixer/speed` | 0.0, 0.5, 1.0 | `GET /control/mixer/speed` | ±0.01 |

### LED panel

| POST | Value(s) | GET | Tolerance |
|------|----------|-----|-----------|
| `POST /control/led_panel/intensity/{ch}` for ch in {0,1,2,3} | 0.0, 0.5, 1.0 | `GET /control/led_panel/intensity/{ch}` | ±0.01 |
| `POST /control/led_panel/intensity` (all channels) | [0,0,0,0], [1,1,1,1], [0.2,0.4,0.6,0.8] | `GET /control/led_panel/intensity/{ch}` for each ch | ±0.01 |

### Pumps module

Instances and pump counts discovered dynamically via `GET /pumps/{i}/pump_count`.

| POST | Value(s) | GET | Tolerance |
|------|----------|-----|-----------|
| `POST /pumps/{i}/speed/{p}` | -1.0, 0.0, 1.0 | `GET /pumps/{i}/speed/{p}` | ±0.01 |
| `POST /pumps/{i}/flowrate/{p}` | 28.0 ml/min | `GET /pumps/{i}/flowrate/{p}` | ±0.01 |

---

## Category 2 - Stop → Zero Output

**Config:** `stop_zero_config.py` | **Test:** `test_stop_zero.py`

After stopping an actuator, the relevant GET endpoints must report zero output.

| Device | Activated with | Stop endpoint | Verified to be zero |
|--------|----------------|---------------|---------------------|
| cuvette_pump | speed=1.0, flowrate=100.0 | `GET /control/cuvette_pump/stop` | speed (±0.01), flowrate (±1.0) |
| aerator | speed=1.0, flowrate=100.0 | `GET /control/aerator/stop` | speed (±0.01), flowrate (±1.0) |
| mixer | speed=1.0 | `GET /control/mixer/stop` | speed (±0.01), rpm (±10), settle 5 s |
| heater | intensity=0.5, target_temperature=37.0 | `GET /control/heater/turn_off` | intensity (±0.01), target_temperature = null |
| pumps | speed=1.0 (all instances/pumps) | `GET /pumps/{i}/stop/{p}` | speed (±0.01), flowrate (±0.01) |

---

## Category 3 - Physical Effects (Power Consumption)

**Config:** `power_draw_config.py` | **Test:** `test_power_draw.py`

Enabling an actuator must increase power consumption measured via
`GET /core/supply/power_draw`. Baseline: 4 samples averaged over 6 s before activation.

| Device | Activated with | Settle | Min increase |
|--------|----------------|--------|--------------|
| heater | intensity=0.8 | 3 s | +15.0 W |
| led_panel | intensity=[1,1,1,1] | 2 s | +8.0 W |
| cuvette_pump | speed=1.0 | 2 s | +2.0 W |
| aerator | speed=1.0 | 2 s | +2.0 W |
| mixer | speed=1.0 | 2 s | +0.5 W |
| pumps (all instances) | speed=1.0 | 2 s | -0.1 W *(TODO: update after real module connected)* |

---

## Category 4 - Thermal Effects of Components

**Config:** `thermal_effects_config.py` | **Test:** `test_thermal_effects.py`

Intensive operation raises component temperatures; idle operation allows them to cool.

### LED panel

| Action | Wait | Verification |
|--------|------|--------------|
| `POST /control/led_panel/intensity` = [1,1,1,1] | 30 s | `GET /control/led_panel/temperature` rises ≥ 3.0 °C |
| Zero out intensity | 60 s | Temperature drops ≥ 3.0 °C |

### Fluorometer emitor

| Action | Verification |
|--------|--------------|
| 20× `POST /sensor/fluorometer/single_sample` (gain=x1, intensity=1.0) | `GET /sensor/fluorometer/emitor/temperature` rises ≥ 1.5 °C |
| Idle 60 s | Temperature drops ≥ 1.5 °C |

### Spectrophotometer emitor

| Action | Verification |
|--------|--------------|
| 10× `POST /sensor/spectrophotometer/measure_all` | `GET /sensor/spectrophotometer/emitor/temperature` rises ≥ 0.5 °C |
| Idle 60 s | Temperature drops ≥ 0.5 °C |

---

## Category 5 - Concurrency & Race Conditions

**Config:** `concurrency_config.py` | **Test:** `test_concurrency.py`

Settle after writes: 0.3 s. Delay between writes: 0.1 s.

### Last write wins

Two sequential POSTs to the same endpoint - GET must return the value from the second write.

| Endpoint | value_a → value_b | GET tolerance |
|----------|-------------------|---------------|
| `POST /control/heater/target_temperature` | 25.0 → 37.0 °C | ±0.01 |
| `POST /control/led_panel/intensity/0` | 0.2 → 0.8 | ±0.01 |
| `POST /control/mixer/speed` | 0.3 → 0.9 | ±0.01 |
| `POST /control/cuvette_pump/speed` | 0.5 → -0.5 | ±0.01 |
| `POST /control/aerator/flowrate` | 100.0 → 500.0 ml/min | ±1.0 |
| `POST /control/aerator/speed` | 0.2 → 0.8 | ±0.01 |
| `POST /control/cuvette_pump/flowrate` | 50.0 → 100.0 ml/min | ±1.0 |
| `POST /pumps/{i}/speed/{p}` (all) | -0.5 → 0.5 | ±0.01 |
| `POST /pumps/{i}/flowrate/{p}` (all) | -28.0 → 28.0 ml/min | ±0.01 |

### Idempotent write (same value twice)

Two identical POSTs - GET must still return the correct value.

| Endpoint | Value | GET tolerance |
|----------|-------|---------------|
| `POST /control/heater/target_temperature` | 30.0 °C | ±0.01 |
| `POST /control/mixer/speed` | 0.6 | ±0.01 |
| `POST /control/led_panel/intensity/0` | 0.5 | ±0.01 |
| `POST /control/cuvette_pump/speed` | 1.0 | ±0.01 |
| `POST /pumps/{i}/speed/{p}` (all) | 0.7 | ±0.01 |
| `POST /pumps/{i}/flowrate/{p}` (all) | 28.0 ml/min | ±0.01 |

---

## Category 6 - State Persistence After Module Restart

**Config:** `state_persistence_config.py` | **Test:** `test_state_persistence.py`

State must be preserved after restarting the control module. Module UID is discovered
dynamically via `GET /system/modules`. Settle after restart: 5 s.

| Scenario | Setup | Verified after restart |
|----------|-------|------------------------|
| heater_target_temperature | `POST /control/heater/target_temperature` = 35.0 °C | target_temperature = 35.0 (±0.01) |
| mixer_speed | `POST /control/mixer/speed` = 0.6 | speed = 0.6 (±0.01) |
| led_panel_all_channels_on | `POST /control/led_panel/intensity` = [1,1,1,1] | each channel intensity = 1.0 (±0.01) |
| led_panel_all_channels_off | `POST /control/led_panel/intensity` = [0,0,0,0] | each channel intensity = 0.0 (±0.01) |

---

## Category 7 - Fluorometer OJIP Workflow

**Config:** `fluorometer_ojip_config.py` | **Test:** `test_fluorometer_ojip.py`

OJIP capture parameters: gain=x1, emitor_intensity=1.0, timebase=logarithmic,
length=1000 ms, sample_count=1000.

| Step | Action | Verification |
|------|--------|--------------|
| 1 | `POST /sensor/fluorometer/ojip/capture` | HTTP 200, response contains data |
| 2 | Poll `GET /sensor/fluorometer/ojip/completed` (timeout 10 s, interval 0.5 s) | `capture_complete` = true |
| 3 | `GET /sensor/fluorometer/ojip/read_last` (first read) | HTTP 200, `read` = false |
| 4 | `GET /sensor/fluorometer/ojip/read_last` (second read) | HTTP 200, `read` = true |

---

## Category 8 - Limits from Info Endpoints

**Config:** `limits_config.py` | **Test:** `test_limits.py`

The info endpoint reports device limits. Tests verify:
1. Setting the device to its reported maximum does not physically exceed that limit.
2. Setting the device above its maximum (×1.1) still does not exceed the limit (clamping).

### Fixed devices

| Device | Info endpoint | Limit field | Set value | GET endpoint | GET field |
|--------|---------------|-------------|-----------|--------------|-----------|
| cuvette_pump | `GET /control/cuvette_pump/info` | max_flowrate | 100 ml/min | `GET /control/cuvette_pump/flowrate` | flowrate |
| mixer | `GET /control/mixer/info` | max_rpm | 6000 RPM | `GET /control/mixer/rpm` | rpm |
| aerator | `GET /control/aerator/info` | max_flowrate | 2500 ml/min | `GET /control/aerator/flowrate` | flowrate |

Over-max factor: **1.1** (configurable via `OVER_MAX_FACTOR`).

### Pumps module

Instances discovered dynamically. For each pump:
- `GET /pumps/{i}/info/{p}` → `max_flowrate`
- Set flowrate to `max_flowrate` → verify `GET /pumps/{i}/flowrate/{p}` ≤ max_flowrate
- Set flowrate to `max_flowrate × 1.1` → verify `GET /pumps/{i}/flowrate/{p}` ≤ max_flowrate

---

## Category 9 - Move → In Progress → Completion / Interruption

**Config:** `move_config.py` | **Test:** `test_move.py`

A move command must be observable during motion and zero after completion or interruption.

### Move completes naturally

| Device | move_body | In-progress check | Wait | Completion check |
|--------|-----------|-------------------|------|------------------|
| cuvette_pump | volume=5.0, flowrate=50.0 | `GET /control/cuvette_pump/flowrate` ≠ 0 (after 0.5 s) | 10 s | `GET /control/cuvette_pump/flowrate` = 0.0 |

### Move interrupted by stop

| Device | move_body | Action | Verification |
|--------|-----------|--------|--------------|
| aerator | volume=500.0, flowrate=100.0 | immediate `GET /control/aerator/stop` | `GET /control/aerator/flowrate` = 0.0 (after 0.5 s) |
| pumps (all instances) | volume=500.0, flowrate=50.0 | immediate `GET /pumps/{i}/stop/{p}` | `GET /pumps/{i}/flowrate/{p}` = 0.0 (after 0.5 s) |

### Stir completes after duration

| Device | stir_body | Wait | Verification |
|--------|-----------|------|--------------|
| mixer | rpm=1000, time=3 s | 6 s | `GET /control/mixer/speed` = 0.0 |

---

## Category 10 - System Statistics and Availability

**Config:** `system_stats_config.py` | **Test:** `test_system_stats.py`

| Test | Action | Verification |
|------|--------|--------------|
| Packets non-zero | `GET /system/can/rx_packets`, `GET /system/can/tx_packets` | both > 0 |
| Packets increasing | Record baseline, trigger `GET /system/modules`, re-read | rx_packets and tx_packets both increased |

Trigger endpoint configurable via `TRIGGER_URL`.

---

## Running Behavioral Tests

```bash
cd tests
source venv/bin/activate

# Run all behavioral tests
pytest behavioral/ -v

# Run a specific category
pytest behavioral/test_set_get.py -v

```
