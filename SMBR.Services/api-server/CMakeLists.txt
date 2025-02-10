cmake_minimum_required(VERSION 3.20)
project(api-server)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME} src/main-api-server.cpp)

target_link_libraries(${PROJECT_NAME} 
  SMBR
  SMBR.REST
  SMBR.Modules.Virtual
  SMBR.Modules.Can
)

add_dependencies(${PROJECT_NAME} 
  SMBR
  SMBR.REST
  SMBR.Modules.Virtual
  SMBR.Modules.Can
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
)



install(TARGETS api-server
  RUNTIME DESTINATION ${SMBR_INSTALL_HOME}
)

#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/api-server.service
#  DESTINATION /etc/systemd/system
#)

#add_custom_target(install-service
#  COMMAND ${CMAKE_COMMAND} -E copy
#          ${CMAKE_CURRENT_SOURCE_DIR}/api-server.service
#          /etc/systemd/system/api-server.service
#  DEPENDS ${PROJECT_NAME}
#)
