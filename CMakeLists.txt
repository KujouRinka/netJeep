cmake_minimum_required(VERSION 3.21)
project(netcat)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Boost 1.66.0 REQUIRED COMPONENTS system program_options)

link_libraries(pthread)

include_directories(common connection proxy/include)
file(GLOB_RECURSE SOURCES "proxy/*.*" connection/*.* common/*.*)

add_executable(netcat main.cc ${SOURCES} connection/connection.cc connection/connection.h)
