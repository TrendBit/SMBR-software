cmake_minimum_required(VERSION 3.20)
project(SMBR.Scheduler)

set(CMAKE_CXX_STANDARD 17)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

add_library(${PROJECT_NAME}
    ${SOURCES}
 )

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC PocoFoundation
  PUBLIC SMBR
)



