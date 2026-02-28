# AGENTS.md

> **Guide for Agentic Coding in the `sips` Project**
> 
> **Audience**: Coding and code review agents.

---

## Table of Contents

1. [Build, Lint, and Test Commands](#build-lint-and-test-commands)
2. [Development Environment](#development-environment)
3. [Code Style Guidelines](#code-style-guidelines)
   - [General Principles](#general-principles)
   - [Project Structure](#project-structure)
   - [File and Import Conventions](#file-and-import-conventions)
   - [Formatting](#formatting)
   - [Naming](#naming)
   - [Types and Declarations](#types-and-declarations)
   - [Error Handling](#error-handling)
   - [Miscellaneous](#miscellaneous)
4. [Butano/Game Boy Advance Specific Practices](#butano-gba-specific-practices)
5. [Agentic Coding Guidance](#agentic-coding-guidance)

---

## Build, Lint, and Test Commands

### Building the Project
- **Build everything:**
  ```sh
  make -j8  # Use the number of logical CPU cores on your machine
  ```
- **Rebuild after changes:**
  ```sh
  make clean && make -j8
  ```
- **Output:**
  - Produces a `.gba` file named after the current directory (e.g. `sips.gba`).
  - All build artifacts go in the `build/` directory.
- **Clean up all build products:**
  ```sh
  make clean
  ```

### Linting
- **C++ Lint/Format:** No linter or formatter is enforced via Makefile. See [Formatting](#formatting) for expectations and recommendations.

### Testing
- **Unit and Integration Testing:**
  - No automated C++/Butano test runners are present in this repo.
  - For logic tests, add test harnesses or test entry points (see example code organization below).
- **Playing/Manual Test:**
  - Run built ROM in a GBA emulator such as [mGBA](https://mgba.io) or [NanoBoyAdvance](https://github.com/nba-emu/NanoBoyAdvance).
  - Typical test workflow:
    1. `make`
    2. Run resulting `.gba` ROM in emulator (e.g. `mgba sips.gba`)
    3. Observe and interact with the game.

### Running a Single Test
- **No explicit single-test command.**
- Agents are encouraged to add test harness sources (e.g., `src/test_foo.cpp`) and Makefile rules if advanced automated testing is needed.

---

## Development Environment

- **Requires:**
  - [devkitARM](https://devkitpro.org/wiki/Getting_Started) properly installed and on `PATH`.
  - Python (2 or 3, for asset and toolchain scripts).
  - GBA emulator for testing/QA (see above for suggestions).
  - [Butano Engine](https://github.com/GValiente/butano) in `../butano/butano` relative to the project directory, unless adjusted in the Makefile.

---

## Code Style Guidelines

### General Principles
- Prefer clarity and maintainability over cleverness.
- Favor modern C++ idioms (e.g., RAII, range-based loops, smart pointers where appropriate, `constexpr`, type-safe enums, strong `namespace` use).
- Avoid: heap allocations, RTTI, and exceptions (these are GBA constraints and Butano best-practices).

### Project Structure
- **Source code:** in `src/`
- **Headers:** in `include/`
- **Assets:** in `graphics/`, `audio/`, etc. (add as needed)

### File and Import Conventions
- Header guards or `#pragma once` on all headers (use `#ifndef/#define` per existing code).
- Group imports: standard library, then external libs (`bn_`), then local headers.
- Example:
  ```cpp
  #include <array>
  #include "bn_sprite_ptr.h"
  #include "my_local_header.h"
  ```
- When including Butano assets, use the auto-generated headers (e.g., `bn_sprite_items_*.h`).

### Formatting
- 4-space indentation; do not use tabs.
- Max line length: 100-120, but prioritize clarity.
- Brace style: opening `{` on same line for functions, methods, and enums.
- Space after comma, before inline comment.
- No trailing whitespace.
- Logical whitespace between function definitions.
- Prefer in-class initialization, member initializer lists in constructors.
- Use C++14+ (as supported by devkitARM and Butano).

### Naming
- Classes, enums, structs: `PascalCase` (e.g., `Person`, `START`).
- Methods, variables, functions: `snake_case`. Constants (constexpr/static const): `UPPER_SNAKE_CASE` if global, or `camelCase` if member.
- Macros: `ALL_CAPS_WITH_UNDERSCORES`.

### Types and Declarations
- Use explicit-width types (`int8_t`, `int32_t`) when crossing GBA boundaries or for asset/data definitions.
- Prefer `auto` for iterator/complex type expressions, but declare fundamental types (`int`, `float`, `bn::fixed`) normally.
- Enums: always declare using `enum class`.
- Use `bn::optional<T>`, `bn::vector`, `bn::deque` for containers—avoid STL heap-based types.

### Error Handling
- GBA/Butano does not use exceptions. Protect against errors with assertions (`BN_ASSERT`, `BN_ERROR`, `bn::log`) and defensive programming.
- Do not allow undefined behavior if possible. Check pointers and container bounds.
- Fallback to safe state if system APIs fail or are missing expected assets.

### Miscellaneous
- Use `namespace`! Avoid polluting global scope.
- Use asset generators and Butano build-in integration for graphics/audio. Regenerate assets when sources change.
- Remove unused code and commented-out blocks unless they're under active development.
- Log debug/diagnostic output using `bn_log`, but remove or guard logs before releases.
- Use Doxygen or similar for documenting public interfaces and complex logic where applicable.

---

## Butano / GBA Specific Practices
- Avoid dynamic memory allocations—use statically sized containers or Butano's `bn::vector`/`bn::optional`.
- Use asset pipeline for images (`graphics/` + Python script -> header).
- Avoid exceptions and RTTI (`-fno-exceptions -fno-rtti`).
- Test game loops in emulators, paying attention to performance (GBA is resource constrained).
- Butano modules used in this project:
  - `bn_sprite_ptr`, `bn_music`, `bn_sound`, `bn_keypad`, `bn_log`, etc.

---

## Agentic Coding Guidance

- Always verify that the `Makefile` and Butano path settings are correct before builds.
- When adding tests, consider making separate test harness cpp files and update the Makefile.
- When formatting or refactoring, follow the conventions above and re-check in the emulator.
- For automation: Suggest GitHub Workflows for build (if cross-platform devkitARM runners are available) but manual QA is expected for gameplay.
- Document any new global constraints, macros, or build options in this file.

---

**If you improve this file, always preserve and extend detailed documentation, workflows, and best practices for agentic and human contributors!**
