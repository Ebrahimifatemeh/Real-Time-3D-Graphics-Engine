# asr 1.2

`asr` is a simple renderer written in C++, designed for creative coding and data visualization. Version 1.2 adds texture mapping.

## Supported Platforms

* Windows 11 (x86-64, ARM)
* macOS Sequoia, Tahoe (x86-64, ARM)
* Ubuntu 24.04 (x86-64, ARM)

## Prerequisites

Ensure all the prerequisites are installed before proceeding.

* MSVC (with Visual Studio 2022 or 2026) on Windows, Clang (with the latest Xcode or Command Line Tools for Xcode) on macOS, or GCC (any version with support for C++20) on GNU/Linux
* vcpkg package manager
* CMake (version 3.28 or higher)
* GPU drivers (latest version with stable support for Vulkan, Metal, or Direct3D 11/12)

Install all prerequisites from your platform's default or official sources.

## Building

1. Configure the project (vcpkg will automatically install dependencies):

    ```bash
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
    ```

    On Ubuntu, install the following system packages first:

    ```bash
    sudo apt install git curl zip unzip tar build-essential pkg-config \
        autoconf autoconf-archive automake libtool                     \
        python3-venv nodejs libltdl-dev                                \
        libx11-dev libxft-dev libxext-dev libx11-xcb-dev               \
        libwayland-dev libxkbcommon-dev                                \
        libxrandr-dev libxinerama-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
        mesa-common-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev                          \
        libdrm-dev libgbm-dev                                                                       \
        libibus-1.0-dev libdbus-1-dev                                                               \
        libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev                           \
        libudev-dev libfribidi-dev libthai-dev
    ```

2. Build the project:

    ```bash
    cmake --build build --config Release # or `Debug` for a build with debugging information
    ```

3. Run the test programs from the `./build/` directory:

    ```bash
    (cd ./build && ./<name of the graphics test executable>) # Note the parentheses
    ```

    You may need to set the Working Directory (CWD) in your IDE manually for some test targets to locate shader or image files.
