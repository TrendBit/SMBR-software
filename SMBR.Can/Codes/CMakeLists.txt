cmake_minimum_required(VERSION 3.20)
project(SMBR.Can.Codes)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
  src/empty.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PUBLIC include/codes/messages 
)

