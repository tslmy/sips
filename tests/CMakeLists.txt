cmake_minimum_required(VERSION 3.14)
project(SipsHelpersTest LANGUAGES CXX)

enable_testing()

include(FetchContent)

# Download Catch2 automatically at configure time
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.0
    FIND_PACKAGE_ARGS 3.7.0
)
FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your tests
add_executable(test_helpers test_helpers.cpp)

# Link Catch2 (v3.x uses proper targets)
target_link_libraries(test_helpers PRIVATE Catch2::Catch2WithMain)

# Include directories - Catch2 v3 uses amalgamated headers in extras
target_include_directories(test_helpers PRIVATE 
    ${catch2_SOURCE_DIR}/extras
    ../include
)

# Add test to ctest
add_test(NAME helpers COMMAND test_helpers)
