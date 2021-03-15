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
- [Files and Directories](#files-and-directories)

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

Build the application:

```shell script
cmake --build build/debug
```

Run cmake with the following options if needed:

* Disassemble any parser statements for debugging using `-DDISASSEMBLE`.
* Disable any logging via `-DDEACTIVATE_LOGGING`.
* Enable detailed execution flow tracing `-DTRACE`

### Release

Build the configuration files with cmake:

```shell script
cmake -GNinja -DCMAKE_BUILD_TYPE=Release --build build/release
```

Build the application:

```shell script
cmake --build build/release
```

### Different compiler

To create builds on a different compiler the variables `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` can be set, specifying the path to the compiler.

Example for clang on MacOS, creating a debug build:

```shell
cmake -DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ --build build/debug
cmake --build build/debug
```

Example for gcc on MacOS, creating a debug build:

```shell
cmake -DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ --build build/debug
cmake --build build/debug
```

## Run

After [building the application](#build) you can either run the client in debug or release mode:

```shell script
# Debug
./build/debug/src/client/Client

# Release
./build/release/src/client/Client
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
cmake --build build/profile
```

Running the profiler executable will generate a `litr-profile.json` file that can be used with any Chromium based browser tracing tool, e.g. [chrome://tracing](chrome://tracing/). Just drag and drop the file into the tracing view. To generate the file run:

```shell script
./build/profile/src/client/Client
```

## Files and Directories

Here an overview of some essential files and folders when working with the code.

### Client

**What:** Contains all client code to run the application inside the terminal.

### Core

**What:** The core library code.

#### Debug

- `src/core/Core/Debug`: Code for debugging the application

#### Error

- `src/core/Core/Error`: Error handling related code
- `src/core/Core/Error/BaseError.hpp`: Base error class

#### Config

- `src/core/Core/Config`: All code related to loading and handling the configuration file
- `src/core/Core/Config/Loader.hpp`: Configuration file loader
- `src/core/Core/Config/Command.hpp`: Configuration command description
- `src/core/Core/Config/Parameter.hpp`: Configuration parameter description

#### CLI

- `src/core/Core/CLI/Parser.hpp`: Command line argument parser
- `src/core/Core/CLI/Interpreter.cpp`: Execute parsed instructions
- `src/core/Core/CLI/Shell.hpp`: Script runner

#### Script

- `src/core/Core/Script/Parser.cpp`: Parse variables and functions in command scripts

### Tests

**What:** All test code for unit and integration tests.

- `src/tests/Fixtures`: Tests fixtures, e.g. test configuration files for integration tests
- `src/tests/Helpers`: Test helper functions
- `src/tests/Tests`: Test cases
