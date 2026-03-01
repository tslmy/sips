# Sips

[![codecov](https://codecov.io/gh/tslmy/sips/branch/main/graph/badge.svg)][cc]

A chill idle game for GBA.

![image](https://github.com/foopod/sips/assets/3768616/04360866-c41f-438f-ad49-8c5f8ac124ec)

![image](https://github.com/foopod/sips/assets/3768616/86a4c0bd-b83e-4313-9199-fe859785978c)

Originally built by [Jono Shields](https://jonoshields.com/) over WinterJam23, this fork has added the following changes:

* UX
    * If you attempt to buy something you can't afford, the cursor will shake and play a sound.
    * The cursor will now skip already-purchased items.
* Dev-only
    * Adopted [Google C++ Style](https://google.github.io/styleguide/cppguide.html), automatically enforced via pre-commit hooks.
    * Replaced vanilla `make` with `CMake` as the build system.
    * Added unit tests. (See "Testing" below.)


## How to play

Press `A` to open wishlist. Move cursor up and down with the D pad. Press `A` again to purchase it, or press `B` to close the wishlist. With more items purchased, your cafe will be more popular (indicated by the bar gauge in the top-left corner).

## Development

This repo uses [pre-commit hooks](https://pre-commit.com/). Ensure you have [prek](https://prek.j178.dev/) installed. After cloning the repo, run `prek install` in the repo's directory before your first commit. You only have to do this per clone.

To build the source code and test code in this repository, you'll need (macOS instructions in parenthesis):

- [CMake](https://cmake.org/) (Available via Homebrew.)
- [devkitARM](https://devkitpro.org/wiki/Getting_Started) (You'll need to install it using their own fork of Pacman.)
- [Butano Engine](https://github.com/GValiente/butano) (Clone it along side with this repo.)
* [just](https://just.systems/) (For simplifying frequently-used commands.)

### Build

To build the GBA ROM, run `just build`. This will create `sips.gba` in the project root, ready for use in a GBA emulator.

### Testing

This project uses [Catch2](https://github.com/catchorg/Catch2) for unit tests. This dependency is managed via [Conan](https://conan.io/). Since Conan is written in Python and I use `uvx` to manage everything with Python, I use `uvx` to run `conan` without explicitly installing it, too.

Ensure you have these commands installed: `just`, `uv`. On macOS, just run `brew install just uv`.

Run `just test` to build and run tests.

**WIP: Code coverage.** I'm still trying to figure out how to make code coverage accurate.

* [The report][cc] is missing `main.cpp`.
* `.github/workflows/coverage.yml` should use `just test` instead of duplicating the commands.

[cc]: https://app.codecov.io/gh/tslmy/sips
