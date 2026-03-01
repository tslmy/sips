cmake_minimum_required(VERSION 3.14)
project(SipsHelpersTest LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3.13.0 REQUIRED)

add_executable(test_helpers
    test_helpers.cpp
    ../src/ti_helpers.cpp
)

target_link_libraries(test_helpers PRIVATE Catch2::Catch2WithMain)

target_include_directories(test_helpers PRIVATE 
    ${Catch2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/host_stubs
    ../include
)

add_test(NAME helpers COMMAND test_helpers)
