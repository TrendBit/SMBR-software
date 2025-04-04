cmake_minimum_required(VERSION 3.20)
project(SMBR.Can.Modules)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
  src/can/CanBus.cpp
  src/can/CanChannel.cpp
  src/can/CanMessage.cpp
  src/can/CanRequest.cpp
  src/can/ICanChannel.cpp
  src/BaseModule.cpp
  src/CanSensorModule.cpp
  src/CanControlModule.cpp
  src/CanCoreModule.cpp
  src/CanCommonModule.cpp
  src/CanSystemModule.cpp
  src/CanModulesFactory.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src
  PRIVATE src/can
)

target_link_libraries(${PROJECT_NAME}
  PocoFoundation
  PocoJSON
  SMBR
  SMBR.Can.Codes
)

