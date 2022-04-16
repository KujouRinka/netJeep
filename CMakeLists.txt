cmake_minimum_required(VERSION 3.16)
project(netJeep)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

file(GLOB_RECURSE TESTS test/*.* config/*.*)
file(GLOB_RECURSE SOURCES "proxy/*.*" connection/*.* common/*.* cipher/*.* router/*.* proxy_manager/*.* config/*.*)
add_executable(
        test_netJeep
        ${TESTS}
)
target_link_libraries(
        test_netJeep
        gtest_main
        gmock_main
        nlohmann_json
)
include(GoogleTest)
gtest_discover_tests(test_netJeep)

FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.tar.gz
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(Boost 1.66.0 REQUIRED COMPONENTS system program_options)

link_libraries(pthread cryptopp)

include_directories(./)

add_executable(netJeep main.cc ${SOURCES})
target_link_libraries(netJeep nlohmann_json)
