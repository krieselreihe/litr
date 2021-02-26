# Contributing

You want do contribute to this repo? Nice! And of course: **you are the best!** Here is a guide on how to work with it and what to expect.

## Contents

- [Required tools](#required-tools)
- [Setup](#setup)
- [Update](#update)
- [Build](#build)
- [Run](#run)
- [Tests](#tests)
- [Profiling](#profiling)

---

## Required tools

### cmake

* Site: https://cmake.org/
* Github: https://github.com/Kitware/CMake

Install on Mac (if not already present):

```shell script
brew install cmake
```

### LLVM

* Site: https://llvm.org/
* Github: https://github.com/llvm/llvm-project

Tools from the LLVM toolchain are used. To install LLVM on Mac run:

```shell script
brew install llvm
```

Tools used from the toolset:

* clang-tidy
* clang-format

To link these tools run:

```shell script
ln -s "$(brew --prefix llvm)/bin/clang-format" "/usr/local/bin/clang-format"
ln -s "$(brew --prefix llvm)/bin/clang-tidy" "/usr/local/bin/clang-tidy"
```

### Ninja

* Site: https://ninja-build.org/
* Github: https://github.com/ninja-build/ninja

You can install Ninja on Mac via brew:

```shell script
brew install ninja
```

## Setup

Clone the project with all submodules:

```shell script
git clone --recurse-submodules -j8 git@github.com:krieselreihe/litr.git
```

## Update

```shell script
git pull && git submodule update --init
```

## Build

### Debug

Build the configuration files with cmake:

```shell script
cmake -GNinja -DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug --build build/debug
```

Build the application with Ninja:

```shell script
ninja -C build/debug
```

### Release

Build the configuration files with cmake:

```shell script
cmake -GNinja -DCMAKE_BUILD_TYPE=Release --build build/release
```

Build the application with Ninja:

```shell script
ninja -C build/release
```

## Run

After [building the application](#build) you can either run the client in debug or release mode:

```shell script
# Debug
./build/debug/litr/client/Client

# Release
./build/release/litr/client/Client
```

## Tests

After [building the application](#build) you can run unit tests for the build output with [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html).

**Note:** With CMake 3.20 it will be possible to specify the `--test-dir` option for ctest [[source](https://cmake.org/cmake/help/latest/release/3.20.html#ctest)], making test execution easier.

```shell script
# Run all tests for debugging build
cd build/debug/src/tests && ctest && ../../../..

# Run all tests for release build
cd build/release/src/tests && ctest && ../../../..
```

## Profiling

There is a profiling build you can generate running cmake with `PROFILE=ON` (build type is up to you, for real world results use "Release"):

```shell script
# Create config files
cmake -GNinja -DPROFILE=ON -DCMAKE_BUILD_TYPE=Release --build build/profile

# Build profile runner
ninja -C build/profile
```

Running the profiler executable will generate a `litr-profile.json` file that can be used with any Chromium based browser tracing tool, e.g. [chrome://tracing](chrome://tracing/). Just drag and drop the file into the tracing view. To generate the file run:

```shell script
./build/profile/src/client/Client
```
