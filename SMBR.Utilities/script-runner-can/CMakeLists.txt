cmake_minimum_required(VERSION 3.20)
project(script-runner-can)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME} src/main-script-runner-can.cpp)

target_link_libraries(${PROJECT_NAME} 
  SMBR
  SMBR.Scheduler
  SMBR.Virtual
  SMBR.Can.Modules
)

add_dependencies(${PROJECT_NAME} 
  SMBR
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
  RUNTIME DESTINATION ${SMBR_INSTALL_HOME}
)
