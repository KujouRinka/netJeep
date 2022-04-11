cmake_minimum_required(VERSION 3.21)
project(netcat)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Boost 1.66.0 REQUIRED COMPONENTS system program_options)

link_libraries(pthread cryptopp)
link_directories("/usr/local/lib")

include_directories(./)
file(GLOB_RECURSE SOURCES "proxy/*.*" connection/*.* common/*.* cipher/*.*)

add_executable(netcat main.cc ${SOURCES})
