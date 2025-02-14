cmake_minimum_required(VERSION 3.20)
project(script-doc-generator)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME} src/script-doc-generator.cpp)

target_link_libraries(${PROJECT_NAME} 
  SMBR
  SMBR.Scheduler
  PocoFoundation
)

add_dependencies(${PROJECT_NAME} 
  SMBR
  SMBR.Scheduler
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
)

install(TARGETS ${PROJECT_NAME} 
  RUNTIME DESTINATION ${SMBR_INSTALL_HOME}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/template.md
  DESTINATION ${SMBR_INSTALL_HOME}/doc/
)
