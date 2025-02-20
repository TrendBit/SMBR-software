cmake_minimum_required(VERSION 3.20)
project(reactor-api-server)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME} src/main-api-server.cpp)

target_link_libraries(${PROJECT_NAME} 
  SMBR
  SMBR.REST
  SMBR.Scheduler
  SMBR.Virtual
  SMBR.Can.Modules
)

add_dependencies(${PROJECT_NAME} 
  SMBR
  SMBR.REST
  SMBR.Scheduler
  SMBR.Virtual
  SMBR.Can.Modules
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
)



install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${SMBR_INSTALL_BIN}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reactor-api-server.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
)

#add_custom_target(install-service
#  COMMAND ${CMAKE_COMMAND} -E copy
#          ${CMAKE_CURRENT_SOURCE_DIR}/api-server.service
#          /etc/systemd/system/api-server.service
#  DEPENDS ${PROJECT_NAME}
#)
