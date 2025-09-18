# Changelog SMBR Api Server

The version number consists of MAJOR.MINOR identifiers.

## 0.3 WorkInProgress 

### Changed
- Change firmware hash output to hexadecimal format

## 0.2

### Added
- New endpoints:
  - `GET /system/errors` – returns a list of current system errors
  - `GET /system/warnings` – returns a list of current system warnings
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
