cmake_minimum_required(VERSION 3.20)
project(reactor-database-export)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/server
  DESTINATION ${SMBR_INSTALL_HOME}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reactor-web-control.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
)
