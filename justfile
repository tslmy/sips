set shell := ["/bin/bash", "-c"]

# justfile for Sips

# Build the GBA ROM
build:
    make -j$(nproc)

# Install dependencies (Conan)
deps:
    cd tests && uvx conan install . --build=missing -s build_type=RelWithDebInfo

# Build test harness
test-build:
    cd tests && \
    cmake -S . -B build/RelWithDebInfo -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE=build/RelWithDebInfo/generators/conan_toolchain.cmake \
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_CXX_FLAGS="--coverage" \
        -DCMAKE_EXE_LINKER_FLAGS="--coverage" && \
    cmake --build build/RelWithDebInfo -j$(nproc)

# Run tests with coverage
test: deps test-build
    cd tests && \
    ./build/RelWithDebInfo/test_helpers && \
    cd build/RelWithDebInfo && \
    rm -f coverage.info && \
    lcov --capture --directory . --output-file coverage.info --ignore-errors inconsistent,source,format && \
    ROOT_DIR=$(cd ../../.. && pwd) && \
    lcov --extract coverage.info "${ROOT_DIR}/src/*" "${ROOT_DIR}/include/*" --output-file coverage.info --ignore-errors inconsistent,corrupt,format,unused && \
    lcov --summary coverage.info --ignore-errors inconsistent,corrupt,format
