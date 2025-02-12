cmake_minimum_required(VERSION 3.20)
project(SMBR.Modules.Virtual)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
  src/Random.cpp
  src/Virtual/VirtualCommonModule.cpp
  src/Virtual/VirtualCoreModule.cpp
  src/Virtual/VirtualControlModule.cpp
  src/Virtual/VirtualSensorModule.cpp
  src/Virtual/VirtualSystemModule.cpp
  src/Virtual/VirtualModulesFactory.cpp
)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS "include/*.hpp")

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC SMBR
)

