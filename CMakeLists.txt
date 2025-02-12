cmake_minimum_required(VERSION 3.20)

#set installation directory
set(SMBR_INSTALL_BASE install_dst)
set(SMBR_INSTALL_HOME ${SMBR_INSTALL_BASE}/home/root/)



project(SMBR.SW)

#build libraries in subdirectories
add_subdirectory(SMBR)
add_subdirectory(SMBR.Modules.Can)
add_subdirectory(SMBR.Modules.Virtual)
add_subdirectory(SMBR.REST)
add_subdirectory(SMBR.Scheduler)

add_subdirectory(SMBR.Services/api-server)
add_subdirectory(SMBR.Services/scheduler)
