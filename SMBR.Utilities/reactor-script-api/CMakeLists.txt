cmake_minimum_required(VERSION 3.20)
project(reactor-script-api)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME} src/main-script-runner-api.cpp)

target_link_libraries(${PROJECT_NAME} 
  SMBR
  SMBR.REST
)

add_dependencies(${PROJECT_NAME} 
  SMBR
  SMBR.REST
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${SMBR_INSTALL_BIN}
)
