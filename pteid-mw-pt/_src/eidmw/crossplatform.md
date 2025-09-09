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


## CMake

To cross compile using cmake, the easiest and standard way is with a toolchain file. This file sets special flags, sucha as what compiler to use and the root path for the target system.

Here is an example:

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)

    // target compilers
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

    // target environment
    set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

    // find_package and find_library flags
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

This toolchain file should be saved and passed as a cmake parameter when configuring the cmake project.

    mkdir build-arm64
    cd build-arm64

    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm64.cmake ..

To compile then simply run:

    make -j$(nproc)
