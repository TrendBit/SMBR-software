# Changelog SMBR Api Server

The version number consists of MAJOR.MINOR identifiers.

## 0.1

### Added
- GET /core/model endpoint for retrieving core model information
- GET /mixer/info endpoint for retrieving mixer information
- GET /aerator/info endpoint for retrieving aerator information
- GET /cuvette_pump/info endpoint for retrieving cuvette pump information
- GET /common/hw_version endpoint for hardware version
- GET /common/fw_version endpoint for firmware version
- Timestamp of OJIP measurement start

### Fixed
- UID matching in restart and bootloader messages

### Removed / Hidden
- Hidden purge and prime cuvette pump endpoints from Swagger UI
