cmake_minimum_required(VERSION 3.14)
project(SipsHelpersTest LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3.13.0 REQUIRED)

add_executable(test_helpers
    test_helpers.cpp
    ../src/ti_helpers.cpp
)

target_link_libraries(test_helpers PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(test_helpers PRIVATE
    TI_HOST_TEST_MATH=1
    BN_CFG_ASSERT_ENABLED=false
)

target_include_directories(test_helpers PRIVATE 
    ${Catch2_INCLUDE_DIRS}
    ../include
    ../external/butano/butano/include
)

add_test(NAME helpers COMMAND test_helpers)
