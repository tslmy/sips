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

if(NOT APPLE)
    add_library(coverage_objects OBJECT
        ../src/wishlist_data.cpp
        ../src/wishlist_menu.cpp
    )
endif()

target_link_libraries(test_helpers PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(test_helpers PRIVATE
    TI_HOST_TEST_MATH=1
    BN_CFG_ASSERT_ENABLED=false
)

if(TARGET coverage_objects)
    target_compile_definitions(coverage_objects PRIVATE
        TI_HOST_TEST_MATH=1
        BN_CFG_ASSERT_ENABLED=false
    )

    target_compile_options(coverage_objects PRIVATE
        -Wno-error
        -Wno-pointer-to-int-cast
        -Wno-shorten-64-to-32
    )
endif()

target_include_directories(test_helpers PRIVATE 
    ${Catch2_INCLUDE_DIRS}
    ../include
    ../build
    ../src
    ../external/butano/butano/include
)

if(TARGET coverage_objects)
    target_include_directories(coverage_objects PRIVATE
        ../include
        ../build
        ../src
        ../external/butano/butano/include
    )
endif()

add_test(NAME helpers COMMAND test_helpers)
