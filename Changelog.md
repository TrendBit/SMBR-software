# Changelog SMBR Api Server

The version number consists of MAJOR.MINOR identifiers.

## 0.9 (WIP)

### Fixed
- `/scheduler/recipe/{recipeName}` - now returns 400 for invalid recipe content
- `/scheduler/` - endpoints now return 409 conflict when attempting to start a script while another is already running
- `/scheduler/recipe/{recipeName}` - selecting a recipe no longer stops an already running recipe

## 0.8

### Added
- Added services ednpoints:
    - post `/services/{service}/disable` - systemd disable
    - post `/services/{service}/enable` - systemd enable
    - post `/services/{service}/restart` - systemd restart
    - post `/services/{service}/stop` - systemd stop
    - post `/services/{service}/start` - systemd start
    - get `/services/{service}/logs` - journalctl output
    - get `/services` - to list all managed service statuses
    - get `/services/{service}` - systemd status
    - post `/services/swupdate/update` - upload and trigger an swu update
    - get `/system/version` - get software version and git info
- Added post `/core/hostname` endpoint for changing the device hostname
- Range limits of endpoints in swagger documentation
- New service reactor-startup-updates 
    - Flashes any modules that don't have the same version as the rpi software
    - Depends on core-module service
    - Api-server service is dependant on it
    - All files in `/firmware/binaries` are now removed and rewritten by new firmwares on swu update
- Serial tag in all endpoints exported with telegram

### Fixed
- Enlarged CAN tx queue for interface can0
- Changed web-control-ts submodule url to new location in Trendbit organization
- Fixed telegram OJIP capture export
    - NTP server in network recommended to export with correct timestamps
- OJIP timesamp not persisting across reboot

## 0.7

### Added
- Expanded behavioral test coverage:
  - Set-get consistency tests for control and pump endpoints
  - Concurrency tests (last-write-wins and idempotent writes)
  - State persistence tests for control module restart scenarios
  - Stop-zero behavior tests for control and pump endpoints
  - Power draw tests
  - Thermal effects tests for LED panel, fluorometer and spectrophotometer
  - OJIP capture workflow tests
  - Limit-from-info tests
  - Move/interruption completion tests
  - System stats tests
- Script for running Schemathesis tests independently

### Changed
- Refactored `run_tests.sh` to improve test execution flow

## 0.6

### Changed
- Added pump module support to common `/{module}/` endpoints:
  - `GET /{module}/ping`
  - `GET /{module}/load`
  - `GET /{module}/core_temp`
  - `GET /{module}/board_temp`
  - `POST /{module}/restart`
  - `GET /{module}/fw_version`
  - `GET /{module}/hw_version`
  - All above endpoints now accept `module=pump` with an optional `instance` query parameter (1–12) that is required when targeting a pump module

## 0.5

### Added
- Automated testing of all endpoints using pytest and Schemathesis (validates expected HTTP status codes)
- `sensor/oled/print_custom_text`: input length limited to 1-127 characters
- `control/heater/target_temperature`: value limited to 0-60°C

### Changed
- Improved validation for POST endpoints
- Refactored and improved consistency of HTTP status codes across the API (e.g., 504 for timeouts, 400 for validation errors, 404 for missing OJIP data)
- Updated Swagger UI documentation for all endpoints
- `GET /recipes/{recipeName}` now uses `recipeName` from the URL instead of the request body
- `POST /scheduler/recipe`: moved `recipeName` from request body to path parameter and rename to `POST /scheduler/recipe/{recipeName}`

## 0.4

### Added
- Support for pump modules in /system/module endpoint
- New endpoints:
  - `GET /pumps/{instance_index}/pump_count` – get information about how many pumps are available in a specific instance of module
  - `GET /pumps/{instance_index}/info/{pump_index}` – get information about a specific pump
  - `GET /pumps/{instance_index}/speed/{pump_index}` - retrieves current speed of the given pump
  - `POST /pumps/{instance_index}/speed/{pump_index}` - sets speed of the pump
  - `GET /pumps/{instance_index}/flowrate/{pump_index}` - retrieves current flow rate of the given pump
  - `POST /pumps/{instance_index}/flowrate/{pump_index}` - sets flow rate of the selected pump
  - `POST /pumps/{instance_index}/calibration/{pump_index}` - calibrates the given pump
  - `POST /pumps/{instance_index}/move/{pump_index}` - moves requested amount of liquid by specified pump
  - `GET /pumps/{instance_index}/stop/{pump_index}` - stops the given pump

## 0.3

### Added
- New endpoints:
  - `POST /control/cuvette_pump/calibration` – configure measured volume value on module in order to calibrate cuvette pump
  - `POST /control/aerator/calibration` – configure measured volume on module in order to calibrate aerator flowrate

### Fixed
- Correct validation ranges for volume and flowrate in control move endpoints

## 0.2

### Added
- New endpoints:
  - `GET /system/errors` – returns a list of current system errors
  - `GET /system/warnings` – returns a list of current system warnings
  - `GET /system/module/issues` – returns a list of current active module issues
  - `GET /system/can/rx_packets` – number of received CAN packets
  - `GET /system/can/tx_packets` – number of transmitted CAN packets
  - `GET /system/can/rx_errors` – count of errors during CAN packet reception
  - `GET /system/can/tx_errors` – count of errors during CAN packet transmission
  - `GET /system/can/rx_dropped` – number of dropped CAN packets during reception
  - `GET /system/can/tx_dropped` – number of dropped CAN packets during transmission
  - `GET /system/can/collisions` – count of CAN bus collisions detected
- Enhanced Swagger UI with more detailed API descriptions

### Changed
- Rename endpoint emitor_temperature to emitor/temperature for consistency
- Renamed parameters in `/sensor/fluorometer/ojip/capture`:
  - `Logarithmic` → `logarithmic`
  - `Linear` → `linear`
  - `timing` → `timebase`
  
### Fixed
- Refactored ojip endpoint:
  - Moved input from query parameters to JSON body
  - Added input validation to prevent malformed requests


## 0.1

### Added
- New endpoints:
    - `GET /core/model` – retrieve core model information
    - `GET /mixer/info` – retrieve mixer information
    - `GET /aerator/info` – retrieve aerator information
    - `GET /cuvette_pump/info` – retrieve cuvette pump information
    - `GET /common/hw_version` – retrieve hardware version
    - `GET /common/fw_version` – retrieve firmware version
- Timestamp of OJIP measurement start

### Fixed
- UID matching in restart and bootloader messages

### Removed / Hidden
- Hidden purge and prime cuvette pump endpoints from Swagger UI
