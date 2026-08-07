cmake_minimum_required(VERSION 3.20)

#set installation directory
set(SMBR_INSTALL_BASE filesystem)
set(SMBR_INSTALL_HOME ${SMBR_INSTALL_BASE}/home/reactor)
set(SMBR_INSTALL_BIN ${SMBR_INSTALL_BASE}/usr/bin)
set(SMBR_INSTALL_SERVICES ${SMBR_INSTALL_BASE}/usr/lib/systemd/system)
set(SMBR_INSTALL_ETC ${SMBR_INSTALL_BASE}/etc)

project(SMBR.SW)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git describe --tags --match "v[0-9]*"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_LAST_TAG
    RESULT_VARIABLE GIT_DESCRIBE_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Handle no tag existence
if(NOT GIT_DESCRIBE_RESULT EQUAL 0)
    set(GIT_LAST_TAG "v0.0")
endif()

# Extract version components
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" SW_VERSION_MAJOR "${GIT_LAST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" SW_VERSION_MINOR "${GIT_LAST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+-([0-9]+).*" "\\1" SW_VERSION_PATCH "${GIT_LAST_TAG}")
if("${SW_VERSION_PATCH}" STREQUAL "${GIT_LAST_TAG}")
    set(SW_VERSION_PATCH 0) # Default to 0 if no patch version is present, e.g. v0.8 with no commit above
endif()

# Extract dirty flag for repo
execute_process(
    COMMAND git status --porcelain
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if("${GIT_STATUS}" STREQUAL "")
    set(SW_GIT_DIRTY false)
else()
    set(SW_GIT_DIRTY true)
endif()

message(STATUS "SW_VERSION_MAJOR: ${SW_VERSION_MAJOR}")
message(STATUS "SW_VERSION_MINOR: ${SW_VERSION_MINOR}")
message(STATUS "SW_VERSION_PATCH: ${SW_VERSION_PATCH}")
message(STATUS "SW_GIT_COMMIT_HASH: ${GIT_COMMIT_HASH}")
message(STATUS "SW_GIT_DIRTY: ${SW_GIT_DIRTY}")

#build libraries in subdirectories
add_subdirectory(SMBR)
add_subdirectory(SMBR.Can/Codes)
add_subdirectory(SMBR.Can/Modules)
add_subdirectory(SMBR.Virtual)
add_subdirectory(SMBR.Scheduler)
add_subdirectory(SMBR.REST)

# SMBR.REST hosts the /system/version endpoint that reports these values
target_compile_definitions(SMBR.REST PRIVATE
    "SW_VERSION_MAJOR=${SW_VERSION_MAJOR}"
    "SW_VERSION_MINOR=${SW_VERSION_MINOR}"
    "SW_VERSION_PATCH=${SW_VERSION_PATCH}"
    "SW_GIT_COMMIT_HASH_STR=\"${GIT_COMMIT_HASH}\""
    "SW_GIT_DIRTY=${SW_GIT_DIRTY}"
)

#build services
add_subdirectory(SMBR.Services/can0)
add_subdirectory(SMBR.Services/reactor-api-server)
add_subdirectory(SMBR.Services/reactor-core-module)
add_subdirectory(SMBR.Services/avahi)
add_subdirectory(SMBR.Services/reactor-database-export)
# Deprecated original reactor-web-control
#add_subdirectory(SMBR.Services/reactor-web-control)
# New typescript web control server
add_subdirectory(SMBR.Services/reactor-web-control-ts)
add_subdirectory(SMBR.Services/reactor-startup-updates)
#utilities
add_subdirectory(SMBR.Utilities/reactor-can-watch)
add_subdirectory(SMBR.Utilities/reactor-firmware-update)
add_subdirectory(SMBR.Utilities/reactor-script-api)
add_subdirectory(SMBR.Utilities/reactor-script-can)
add_subdirectory(SMBR.Utilities/script-doc-generator)

#tests
#add_subdirectory(SMBR.Utilities/scheduler)
