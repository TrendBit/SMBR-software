cmake_minimum_required(VERSION 3.20)
project(reactor-startup-updates)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reactor-startup-updates.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
)
