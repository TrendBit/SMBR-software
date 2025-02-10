cmake_minimum_required(VERSION 3.20)
project(SMBR)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
  src/Exceptions.cpp
  src/ICoreModule.cpp
  src/ISystemModule.cpp
  src/SMBR.cpp
 )

file(GLOB_RECURSE SMBR_HEADERS "include/*.hpp")

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC PocoFoundation
)

#install so library and include files
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
