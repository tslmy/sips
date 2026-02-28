# Sips

A chill idele game for GBA WinterJam23

![image](https://github.com/foopod/sips/assets/3768616/04360866-c41f-438f-ad49-8c5f8ac124ec)

![image](https://github.com/foopod/sips/assets/3768616/86a4c0bd-b83e-4313-9199-fe859785978c)

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
