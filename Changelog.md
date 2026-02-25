# Changelog SMBR Api Server

The version number consists of MAJOR.MINOR identifiers.

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
