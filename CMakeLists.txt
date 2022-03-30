cmake_minimum_required(VERSION 3.21)
project(netcat)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Boost 1.66.0 REQUIRED COMPONENTS system program_options)

link_libraries(pthread)
file(GLOB_RECURSE SOURCES "Proxy/*.*")

add_executable(netcat main.cc ${SOURCES})
