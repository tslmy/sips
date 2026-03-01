set shell := ["/bin/bash", "-c"]

# justfile for Sips

# Build the GBA ROM
build:
    make -j$(nproc)

# Install dependencies (Conan)
deps:
    cd tests && uvx conan install . --build=missing

# Build test harness
test-build:
    cd tests && \
    mkdir -p build && \
    cd build && \
    cmake .. -G "Unix Makefiles" \
        -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake \
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
        -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -j$(nproc)

# Run tests with coverage
test: deps test-build
    cd tests && ./build/test_helpers