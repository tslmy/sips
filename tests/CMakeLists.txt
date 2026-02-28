cmake_minimum_required(VERSION 3.14)
project(SipsHelpersTest LANGUAGES CXX)

include(FetchContent)

# Download Catch2 automatically at configure time
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.10
)
FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your tests
add_executable(test_helpers test_helpers.cpp)

# Link Catch2 header interface target (v2.x)
# For v2.x, manually specify include dir for single header:
target_include_directories(test_helpers PRIVATE ${catch2_SOURCE_DIR}/single_include)

# Add test to ctest
add_test(NAME helpers COMMAND test_helpers)
