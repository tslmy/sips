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
    set(COVERAGE_GENERATED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_regular_bg_items_overlay.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_cursor.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_bonsai.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_cactus1.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_clock.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_cookies.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_painting.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_sylvester.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_topiary.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_typist.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../build/bn_sprite_items_vines.h"
    )

    set(ENABLE_COVERAGE_OBJECTS TRUE)
    foreach(GENERATED_HEADER IN LISTS COVERAGE_GENERATED_HEADERS)
        if(NOT EXISTS "${GENERATED_HEADER}")
            set(ENABLE_COVERAGE_OBJECTS FALSE)
            break()
        endif()
    endforeach()

    if(ENABLE_COVERAGE_OBJECTS)
        add_library(coverage_objects OBJECT
            ../src/wishlist_data.cpp
            ../src/wishlist_menu.cpp
        )
    else()
        message(STATUS "Skipping coverage_objects: missing generated asset headers in ../build")
    endif()
endif()

target_link_libraries(test_helpers PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(test_helpers PRIVATE
    BN_CFG_ASSERT_ENABLED=false
)

if(TARGET coverage_objects)
    target_compile_definitions(coverage_objects PRIVATE
        BN_CFG_ASSERT_ENABLED=false
    )

    target_compile_options(coverage_objects PRIVATE
        -Wno-error
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
