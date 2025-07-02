cmake_minimum_required(VERSION 3.20)
project(reactor-core-module)

set(CMAKE_CXX_STANDARD 17)



execute_process(
    COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR}/src rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR}/src describe --tags
    OUTPUT_VARIABLE GIT_LAST_TAG
    RESULT_VARIABLE RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Handle no tag existence
if(NOT RESULT EQUAL 0)
    set(GIT_LAST_TAG "v0.0")
endif()

# Extract version components
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" FW_VERSION_MAJOR "${GIT_LAST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" FW_VERSION_MINOR "${GIT_LAST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+-([0-9]+).*" "\\1" FW_VERSION_PATCH "${GIT_LAST_TAG}")
if("${FW_VERSION_PATCH}" STREQUAL "${GIT_LAST_TAG}")
    set(FW_VERSION_PATCH 0) # Default to 0 if no patch version is present v0.8 with no commit above
endif()

# Extract dirty flag for repo
execute_process(
    COMMAND git status --porcelain
    OUTPUT_VARIABLE GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if("${GIT_STATUS}" STREQUAL "")
    set(FW_GIT_DIRTY false)
else()
    set(FW_GIT_DIRTY true)
endif()

# Print extracted variables to the terminal
message(STATUS "FW_VERSION_MAJOR: ${FW_VERSION_MAJOR}")
message(STATUS "FW_VERSION_MINOR: ${FW_VERSION_MINOR}")
message(STATUS "FW_VERSION_PATCH: ${FW_VERSION_PATCH}")
message(STATUS "FW_GIT_COMMIT_HASH: ${GIT_COMMIT_HASH}")
message(STATUS "FW_GIT_DIRTY: ${FW_GIT_DIRTY}")


# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "src/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add definitions for compilation
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DFW_VERSION_MAJOR=${FW_VERSION_MAJOR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DFW_VERSION_MINOR=${FW_VERSION_MINOR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DFW_VERSION_PATCH=${FW_VERSION_PATCH}")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DFW_GIT_COMMIT_HASH_STR=\"${GIT_COMMIT_HASH}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DFW_GIT_COMMIT_HASH_HEX=0x${GIT_COMMIT_HASH}")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DFW_GIT_DIRTY=${FW_GIT_DIRTY}")


target_include_directories(${PROJECT_NAME}
  PRIVATE src/source
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
