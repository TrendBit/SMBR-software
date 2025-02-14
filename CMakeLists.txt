cmake_minimum_required(VERSION 3.20)

#set installation directory
set(SMBR_INSTALL_BASE filesystem)
set(SMBR_INSTALL_HOME ${SMBR_INSTALL_BASE}/home/root/)
set(SMBR_INSTALL_BIN ${SMBR_INSTALL_BASE}/usr/local/bin/)
set(SMBR_INSTALL_SERVICES ${SMBR_INSTALL_BASE}/etc/systemd/system/)
set(SMBR_INSTALL_ETC ${SMBR_INSTALL_BASE}/etc/)

project(SMBR.SW)

#build libraries in subdirectories
add_subdirectory(SMBR)
add_subdirectory(SMBR.Can/Codes)
add_subdirectory(SMBR.Can/Modules)
add_subdirectory(SMBR.Virtual)
add_subdirectory(SMBR.Scheduler)
add_subdirectory(SMBR.REST)

#build services
add_subdirectory(SMBR.Services/can0)
add_subdirectory(SMBR.Services/smbr-api-server)
add_subdirectory(SMBR.Services/smbr-core-module)
add_subdirectory(SMBR.Services/avahi)

#utilities
add_subdirectory(SMBR.Utilities/script-runner-api)
add_subdirectory(SMBR.Utilities/script-runner-can)

#tests
add_subdirectory(SMBR.Utilities/scheduler)
