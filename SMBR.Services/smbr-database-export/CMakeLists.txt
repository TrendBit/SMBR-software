cmake_minimum_required(VERSION 3.20)
project(smbr-database-export)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/telegraf.d
  DESTINATION ${SMBR_INSTALL_HOME}/database-export
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/smbr-database-export.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
)
