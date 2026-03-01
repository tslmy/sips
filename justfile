set shell := ["/bin/bash", "-c"]

# justfile for Sips

_jobs := "$(command -v nproc >/dev/null 2>&1 && nproc || sysctl -n hw.logicalcpu)"

# Initialize/update git submodules (including Butano at external/butano)
setup-butano:
    git submodule update --init --recursive

# Bootstrap all dependencies needed for local development
setup: setup-butano

# Build the GBA ROM
build:
    make -j{{_jobs}}

# Install dependencies (Conan)
deps:
    cd tests && \
    rm -rf build/RelWithDebInfo && \
    uvx conan install . --build=missing -s build_type=RelWithDebInfo -s compiler.cppstd=23

# Build test harness
test-build:
    cd tests && \
    cmake -S . -B build/RelWithDebInfo -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE=build/RelWithDebInfo/generators/conan_toolchain.cmake \
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_CXX_FLAGS="--coverage" \
        -DCMAKE_EXE_LINKER_FLAGS="--coverage" && \
    cmake --build build/RelWithDebInfo -j{{_jobs}}

# Run tests with coverage
test: deps test-build
    cd tests && \
    ./build/RelWithDebInfo/test_helpers && \
    cd build/RelWithDebInfo && \
    rm -f coverage.info && \
    lcov --capture --directory . --output-file coverage.info --ignore-errors inconsistent,source,format && \
    ROOT_DIR=$(cd ../../.. && pwd) && \
    lcov --extract coverage.info "${ROOT_DIR}/src/*" "${ROOT_DIR}/include/*" --output-file coverage.info --ignore-errors inconsistent,corrupt,format,unused && \
    lcov --remove coverage.info "${ROOT_DIR}/external/*" --output-file coverage.info --ignore-errors inconsistent,corrupt,format,unused,unused && \
    lcov --summary coverage.info --ignore-errors inconsistent,corrupt,format
