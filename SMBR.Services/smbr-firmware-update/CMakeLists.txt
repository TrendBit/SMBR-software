cmake_minimum_required(VERSION 3.20)
project(smbr-firmware-update)


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/smbr-firmware-update.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
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
