# litr

**L**anguage **I**ndependent **T**ask **R**unner, configure and then run any tasks you want.

**Note:** This repository only serves for defining the tool, for now. Implementation will follow.

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
cmake -GNinja -DDEBUG=1 -DCMAKE_BUILD_TYPE=Debug --build build/debug
```

Build the application with Ninja:

```shell script
ninja -C build/debug
```

### Release

Build the configuration files with cmake:

```shell script
cmake -GNinja -DDEBUG=0 -DCMAKE_BUILD_TYPE=Release --build build/release
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
