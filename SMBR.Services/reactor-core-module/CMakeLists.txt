cmake_minimum_required(VERSION 3.20)
project(reactor-core-module)

set(CMAKE_CXX_STANDARD 17)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "src/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
  PRIVATE src
)

target_link_libraries(${PROJECT_NAME} 
  SMBR.Can.Codes
  PocoFoundation
  PocoCrypto
  systemd
)

add_dependencies(${PROJECT_NAME} 
  SMBR.Can.Codes
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
)



install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${SMBR_INSTALL_BIN}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reactor-core-module.service
  DESTINATION ${SMBR_INSTALL_SERVICES}
)

#add_custom_target(install-service
#  COMMAND ${CMAKE_COMMAND} -E copy
#          ${CMAKE_CURRENT_SOURCE_DIR}/api-server.service
#          /etc/systemd/system/api-server.service
#  DEPENDS ${PROJECT_NAME}
#)
