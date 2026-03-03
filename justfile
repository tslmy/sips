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
    uvx conan profile detect --force && \
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
    cd tests/build/RelWithDebInfo
    rm -f coverage.base.raw.info coverage.run.raw.info coverage.base.info coverage.run.info coverage.info

    # We capture a `coverage.base.raw.info` first to account for all code files. This report will say all files have 0% coverage.
    lcov --capture --initial --directory . --output-file "coverage.base.raw.info" --ignore-errors inconsistent,source,format
    
    # Then we run the tests and capture `coverage.run.raw.info` to see what was actually executed. Since `./test_helpers` is built with only a subset of project files,
    # a lot of them will simply not appear in this report.
    ./test_helpers
    ./test_people
    lcov --capture --directory . --output-file "coverage.run.raw.info" --ignore-errors inconsistent,source,format
    
    # Both `*.raw.info` files will include code from `external/` and `tests/`, 
    # so we then filter them to only include source and header files from the main project (`src/` and `include/`).
    ROOT_DIR=$(cd ../../.. && pwd)
    lcov --extract "coverage.base.raw.info" \
         "${ROOT_DIR}/src/*" "${ROOT_DIR}/include/*" \
         --output-file "coverage.base.info" \
         --ignore-errors inconsistent,corrupt,format,unused
    lcov --extract "coverage.run.raw.info" \
         "${ROOT_DIR}/src/*" "${ROOT_DIR}/include/*" \
         --output-file "coverage.run.info" \
         --ignore-errors inconsistent,corrupt,format,unused

    # Finally, we combine the filtered base and run reports into `coverage.info` and print a summary.
    lcov --add-tracefile "coverage.base.info" \
         --add-tracefile "coverage.run.info" \
         --output-file "coverage.info" \
         --ignore-errors inconsistent,source,format
    lcov --summary "coverage.info" --ignore-errors inconsistent,corrupt,format
