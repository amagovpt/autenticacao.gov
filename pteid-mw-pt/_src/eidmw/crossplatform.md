# Crossplatform compilation

## Prerequirements

First a compiler is needed for crosscompilation, that targets the desired arch. For example ARM64 (AArch64):

    sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

The project dependencies should also be compiled for the target architecture and installed on the target environment sysroot.
This can be both be achieved by compiling from source and installing them on our target sysroot. When compiling the dependency it's important to set the target architecture and the instalation prefix.

This is how to configure openssl for arm64:

    ./Configure linux-aarch64 --cross-compile-prefix=aarch64-linux-gnu- --prefix=/usr/aarch64-linux-gnu --openssldir=/usr/aarch64-linux-gnu/ssl

This way we configure openssl build system to compile using arm64 compiler and install itself (with `sudo make install`) in `/usr/aarch64-linux-gnu`.
All dependencies should be installed on the same sysroot.

## PCSC
To compile the project without PCSC as a dependency, the flag USE_PCSC (`-DUSE_PCSC=<bool>`) can be set during cmake configuration. This will also disable some other features such as pinpad support.




## CMake

To cross compile using cmake, the easiest and standard way is with a toolchain file. This file sets special flags, such as what compiler to use and the root path for the target system.

Here is an example toolchain file for ARM64:

    # CMake Toolchain File for ARM64 (aarch64) Cross-Compilation
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)

    # Specify the cross compiler
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
    set(CMAKE_STRIP aarch64-linux-gnu-strip)

    # Target environment - where to find libraries and headers
    set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
    
    # Set sysroot and prefix path to help CMake find libraries
    set(CMAKE_SYSROOT /usr/aarch64-linux-gnu)
    set(CMAKE_PREFIX_PATH /usr/aarch64-linux-gnu)
    
    # Set default install prefix to the sysroot
    set(CMAKE_INSTALL_PREFIX /usr/aarch64-linux-gnu CACHE PATH "Install path prefix" FORCE)

    # Add library search path for linker
    set(CMAKE_EXE_LINKER_FLAGS_INIT "-L/usr/aarch64-linux-gnu/lib")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "-L/usr/aarch64-linux-gnu/lib")

    # Search for programs in the build host directories
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    
    # Search for libraries and headers ONLY in the target directories
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

This toolchain file should be saved (e.g., as `toolchain-aarch64.cmake`) and passed as a cmake parameter when configuring the cmake project.

**Important:** The toolchain file must be specified during the initial configuration. If reconfiguring an existing build directory, remove it first:

    rm -rf build
    cmake -S . -B build \
      -DCMAKE_TOOLCHAIN_FILE=toolchain-aarch64.cmake \
      -DUSE_PCSC=false \
      -DCMAKE_BUILD_TYPE=Release

To compile:

    cmake --build build -j$(nproc)

To install to the toolchain sysroot (using the `CMAKE_INSTALL_PREFIX` from the toolchain file):

    cmake --install build
