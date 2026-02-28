# AGENTS.md

> Guide for agentic coding and automated contributors working in this repository.
>
> Audience: agent assistants, code-review bots, and human contributors.

---

## Contents

1. Build, lint, and test commands
2. Development environment
3. Code style and conventions
4. Butano / GBA specifics
5. Agent guidelines and workflow

---

## Build, Lint, and Test Commands

- Build (recommended):
  - If a Makefile exists: `make -j$(nproc)` (or `make -j8` on macOS use `sysctl -n hw.logicalcpu`).
  - If there is no project Makefile, the repo expects `devkitARM`/Butano; see Development Environment below.

- Rebuild clean:
  - `make clean && make -j$(nproc)`

- Clean only:
  - `make clean`

- Output:
  - Typical output is a GBA ROM named after the directory (e.g. `sips.gba`) and intermediate build artifacts in `build/`.

- Lint / Format:
  - There is no enforced formatter. Use clang-format with a project style if you add one. Prefer 4-space indentation and column width 100–120.

- Running tests (existing repo):
  - This repository does not include automated test runners by default. Two testing approaches are common:
    1) Host-unit tests (fast): Add test sources that avoid hardware/Butano APIs and compile them for the host (g++/clang++) so they run locally.
    2) GBA integration tests: build as a ROM and run in an emulator (mgba, NanoBoyAdvance).

- Recommended pattern for single test targets:
  - Name tests `src/test_*.cpp` and add Makefile rules; example snippet to add to a Makefile:

```makefile
# build host test binary (avoids Butano-specific code)
HOST_TEST_SRCS := $(wildcard src/test_*.cpp)
HOST_TEST_BIN := $(patsubst src/%.cpp,bin/%,$(HOST_TEST_SRCS))

	$(CXX) $(CXXFLAGS) -Iinclude -o $@ $<

bin:
	mkdir -p bin

# build a single test: make bin/test_foo
```

- To run a single test once you add the rule: `make bin/test_foo && ./bin/test_foo`

- GBA test harnesses:
  - Create `src/test_harness.cpp` that initializes minimal Butano state and exposes a frame loop for assertions. Add Makefile target `test-gba` that builds a `.gba` and run with `mgba build/sips.gba`.

---

## Development Environment

- Required for building GBA ROMs:
  - devkitARM (devkitPro). Ensure `devkitARM/bin` is on PATH.
  - Python 2/3 for asset scripts and header generation.
  - Butano engine checked out at `../butano/butano` (or update Makefile include paths).
  - GBA emulator (mgba recommended) for manual QA.

- Verify paths before building; failing to point to Butano is the most common error.

---

## Code Style and Conventions

- General principles:
  - Clarity and maintainability over cleverness; prefer explicit, small functions.
  - Use RAII and modern C++ idioms where possible while avoiding heap allocations at runtime on the GBA.

- File layout:
  - Sources: `src/`
  - Public headers: `include/`
  - Generated assets: `graphics/`, `audio/`, etc.

- Header and include rules:
  - Use `#ifndef/#define/#endif` header guards or `#pragma once` consistently.
  - Order includes: 1) project-local config, 2) C++ stdlib, 3) third-party (Butano `bn_*`), 4) local headers.
  - Keep include lists minimal in headers; prefer forward declarations when possible.

- Import example:
```cpp
#include <array>
#include "bn_common.h"        // Butano core includes
#include "bn_sprite_ptr.h"
#include "my_project/foo.h"  // local header
```

- Formatting rules:
  - Indent with 4 spaces; do not use tabs.
  - Max line length 100–120 characters; break long expressions across multiple lines.
  - Place opening brace on same line for functions and control blocks.
  - Use consistent spacing and blank lines between logical blocks.

- Naming conventions:
  - Types (classes/structs/enums): PascalCase (e.g., `Person`, `GameState`).
  - Functions, methods, variables: snake_case (e.g., `update_score`).
  - Global constants: UPPER_SNAKE_CASE. Member constants may use camelCase.
  - Macros: ALL_CAPS_WITH_UNDERSCORES.

- Types and declarations:
  - Use fixed-width integers for cross-platform or hardware-facing code: `int8_t`, `uint16_t`, `int32_t`.
  - Use `auto` for iterators and long types, but prefer explicit fundamental types for API clarity.
  - Use `enum class` for enums.
  - Prefer Butano containers (`bn::vector`, `bn::optional`) to avoid dynamic heap allocations.

- Error handling and assertions:
  - Do not use C++ exceptions for runtime error handling on GBA builds.
  - Use `BN_ASSERT`, `BN_ERROR`, and `bn::log` for diagnostics and guard clauses.
  - Validate input and check container bounds; avoid undefined behaviour.
  - For host-only tests you may use `assert()` or exceptions locally; guard test-only code with `#ifdef HOST_TEST`.

- Documentation:
  - Use short doxygen-style comments for public interfaces and non-obvious logic.

---

## Butano / GBA Specific Practices

- Avoid runtime heap allocations. Prefer compile-time arrays or `bn` containers.
- Build flags: use `-fno-exceptions -fno-rtti` for GBA builds.
- Asset pipeline: regenerate and check-in generated headers from `graphics/` sources when assets change.
- Performance: keep per-frame work minimal; avoid complex floating point math and heavy memory copies.

---

## Agent Guidelines and Workflow

- When making commits: do not amend or force push unless explicitly requested. Do not change unrelated files.
- If adding tests: add `src/test_*.cpp` and a Makefile rule as shown above; prefer host tests for fast CI feedback.
- If you need secrets or credentials, ask the human—do not hardcode.
- Search and modify code with care: prefer small, atomic changes with clear commit messages.

- Cursor / Copilot rules:
  - There are no `.cursor/rules/` or `.cursorrules` directories in this repository.
  - There is no `.github/copilot-instructions.md` present. If you add repository-level Copilot or Cursor rules, document them here and preserve them when editing this file.

---

If you update this file, preserve the guidance for build paths, Butano configuration, and test harness naming. Keep the test pattern `src/test_*.cpp` and the `bin/` host test target so agent workflows remain predictable.
