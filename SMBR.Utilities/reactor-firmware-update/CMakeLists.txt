cmake_minimum_required(VERSION 3.20)
project(reactor-firmware-update)


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/update_firmware.sh
  DESTINATION ${SMBR_INSTALL_HOME}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ
)

#copy all files from firmware subfolder to the install directory
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/firmware
  DESTINATION ${SMBR_INSTALL_HOME}
)

#add_custom_target(install-service
#  COMMAND ${CMAKE_COMMAND} -E copy
#          ${CMAKE_CURRENT_SOURCE_DIR}/api-server.service
#          /etc/systemd/system/api-server.service
#  DEPENDS ${PROJECT_NAME}
#)
