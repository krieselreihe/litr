# Contributing

You want to contribute to this repo? Nice! And of course: **you are the best!** Here is a guide on how to work with it and what to expect.

## Contents

- [Required tools](#required-tools)
- [Setup](#setup)
- [Update](#update)
- [Build](#build)
- [Tests](#tests)
- [Profiling](#profiling)
- [Usage without Litr](#usage-without-litr)
- [Run](#run)
- [Files and Directories](#files-and-directories)

---

## Required tools

### cmake

CMake is an open-source, cross-platform family of tools designed to build, test and package software.

* Site: https://cmake.org/
* Github: https://github.com/Kitware/CMake

Install on macOS (if not already present):

```shell
brew install cmake
```

### LLVM

The LLVM Project is a collection of modular and reusable compiler and toolchain technologies. Some tools from the LLVM toolchain are used.

* Site: https://llvm.org/
* Github: https://github.com/llvm/llvm-project

To install LLVM on macOS run:

```shell
brew install llvm
```

Tools used from the toolset:

* clang-tidy (linting)
* clang-format (formatting)

To link these tools run (may need to be run as root, you can also pick a different location in your `PATH`):

```shell
ln -s "$(brew --prefix llvm)/bin/clang-format" "/usr/local/bin/clang-format"
ln -s "$(brew --prefix llvm)/bin/clang-tidy" "/usr/local/bin/clang-tidy"
```

### Ninja

Ninja is a small build system with a focus on speed.

* Site: https://ninja-build.org/
* Github: https://github.com/ninja-build/ninja

You can install Ninja on macOS via brew:

```shell
brew install ninja
```

### Litr

Of course this repo uses Litr itself for its tasks. So installing it will make things easier. To install Litr:

```shell
brew tap krieselreihe/litr
brew install litr
```

## Setup

Clone the project:

```shell
git clone git@github.com:krieselreihe/litr.git
```

## Update

```shell
litr update
```

## Build

For a quick overview of build options run `litr build --help`.

### Debug

Build a debug build:

```shell
litr build
```

Run Litr with the following options if needed:

* Enable detailed execution flow tracing including disassemble any parser statements `litr build --trace`
* Disable any logging via `litr build --nolog`.
* Set debug mode, even if build type differs (for debugging purposes) `litr build --debug`

### Release

Build a release version:

```shell
litr build --target=release
```

For a release that can be published, and the release process in full, visit the [wiki](https://github.com/krieselreihe/litr/wiki/Release).

## Tests

After [building the application](#build) you can run unit tests for the build output with [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html).

```shell
# Run all tests for debugging build
litr test

# Run all tests for release build
litr test --target=release
```

## Profiling

There is a profiling build you can generate (build target is up to you, for real world results use "release"):

```shell
litr build --target=release --profile
```

Running the executable directly will generate a `litr-profile.json` file that can be used with any Chromium based browser tracing tool, e.g. [chrome://tracing](chrome://tracing/). Just drag and drop the file into the tracing view. To generate the file run the local build directly:

```shell
./build/release/src/client/Client
```

## Usage without Litr

<details>
  <summary>Click to expand</summary>

## Update without Litr

```shell
git pull
```

## Build without Litr

### Debug without Litr

Build the configuration files with cmake:

```shell
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug --build build/debug
```

Build the application:

```shell
cmake --build build/debug
```

Run cmake with the following options if needed:

* Disassemble any parser statements for debugging using `-DDISASSEMBLE=ON`.
* Disable any logging via `-DDEACTIVATE_LOGGING=ON`.
* Enable detailed execution flow tracing `-DTRACE=ON`
* Set debug mode, even if build type differs (for debugging purposes) `-DDEBUG=ON`

### Release without Litr

Build the configuration files with cmake:

```shell
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build/release
```

Build the application:

```shell
cmake --build build/release
```

## Tests without Litr

After [building the application](#build) you can run unit tests for the build output with [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html).

**Note:** With CMake 3.20 it will be possible to specify the `--test-dir` option for ctest [[source](https://cmake.org/cmake/help/latest/release/3.20.html#ctest)], making test execution easier.

```shell
# Run all tests for debugging build
cd build/debug/src/tests && ctest && ../../../..

# Run all tests for release build
cd build/release/src/tests && ctest && ../../../..
```

## Profiling without Litr

There is a profiling build you can generate running cmake with `PROFILE=ON` (build type is up to you, for real world results use "Release"):

```shell
# Create config files
cmake -GNinja -DPROFILE=ON -DCMAKE_BUILD_TYPE=Release --build build/profile

# Build profile runner
cmake --build build/profile
```

Running the profiler executable will generate a `litr-profile.json` file that can be used with any Chromium based browser tracing tool, e.g. [chrome://tracing](chrome://tracing/). Just drag and drop the file into the tracing view. To generate the file run:

```shell
./build/profile/src/client/Client
```

</details>

### Different compiler

To create builds on a different compiler the variables `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` can be set, specifying the path to the compiler.

Example for clang on macOS, creating a debug build:

```shell
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -B build/debug
cmake --build build/debug
```

Example for gcc on macOS, creating a debug build:

```shell
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ -B build/debug
cmake --build build/debug
```

## Run

After [building the application](#build) you can either run the local client:

```shell
# Debug
./build/debug/src/client/Client

# Release
./build/release/src/client/Client
```

## Files and Directories

Here an overview of **a few** important files and folders when working with the code.

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

- `src/core/Core/Script/Compiler.cpp`: Compile CLI instructions to a valid executable script

### Tests

**What:** All test code for unit and integration tests.

- `src/tests/Fixtures`: Tests fixtures, e.g. test configuration files for integration tests
- `src/tests/Helpers`: Test helper functions
- `src/tests/Tests`: Test cases
