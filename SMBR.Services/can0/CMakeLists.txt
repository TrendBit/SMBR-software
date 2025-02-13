cmake_minimum_required(VERSION 3.20)
project(can0)

set(CMAKE_CXX_STANDARD 17)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/can0.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
)
