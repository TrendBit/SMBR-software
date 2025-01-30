cmake_minimum_required(VERSION 3.20)
project(SMBR.REST)

set(CMAKE_CXX_STANDARD 17)

# --- ServerLibrary (oatpp-server) ---
find_package(oatpp 1.4.0 REQUIRED)
find_package(oatpp-swagger 1.4.0 REQUIRED)


add_definitions(
  -DOATPP_SWAGGER_RES_PATH="/usr/local/include/oatpp-1.4.0/bin/oatpp-swagger/res"
)

add_library(${PROJECT_NAME}
  src/AppComponent.hpp
  src/SMBRController.cpp
  src/SMBRServer.cpp
)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS "include/*.hpp")

target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/dto
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC SMBR
  PUBLIC oatpp::oatpp
  PUBLIC oatpp::oatpp-swagger
)


#install so library and include files
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Target
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
