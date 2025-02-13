cmake_minimum_required(VERSION 3.20)
project(avahi)

set(CMAKE_CXX_STANDARD 17)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bioreactor.service
  DESTINATION ${SMBR_INSTALL_ETC}/avahi/services/
)

