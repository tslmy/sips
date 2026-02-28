# Sips

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

### Build

To build the GBA ROM, run:

```
make -j8
```

This will create `sips.gba` in the project root, ready for use in a GBA emulator.

### Testing

This project uses [Catch2](https://github.com/catchorg/Catch2) for unit tests.

1. **Build the host-side test harness:**

    ```sh
    cd tests
    cmake -S . -B build
    cmake --build build
    ```

2. **Run the tests:**

    ```sh
    ./build/test_helpers
    ```

    All logic tests will execute and print results in the terminal.
