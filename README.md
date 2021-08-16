# litr

Litr (_Language Independent Task Runner_), lets you configure and then run any tasks you want for any language. Define scripts, run executables or execute commands on your machine. Litr serves as an abstraction layer to combine different strategies into one tool.

**Attention:** This software is currently in alpha ⚠️

## Quick Setup

Install the tool via [Homebrew](https://brew.sh/). Currently, a simple installation is only supported under **macOS** (universal binary for intel and silicon). For other platforms you need to [build from source](CONTRIBUTING.md). Support for other platforms will follow [#37, #38].

```shell
# On macOS
brew tap krieselreihe/litr
brew install litr
```

Define some commands, to make it feel real the example wraps this project with C++ and CMake:

```toml
[commands]
update = "git pull"
build = [
  "cmake -GNinja -DCMAKE_BUILD_TYPE=%{target} -B build/%{target}",
  "ninja -C build/%{target}"
]
test = "cd build/%{target}/src/tests && ctest"

[params.target]
shortcut = "t"
description = "Define the application build target."
type = ["debug", "release"]
default = "debug"
```

Run them all:

```shell
litr --target=debug update,build,test
```

To find out more go to the [wikis setup section](https://github.com/krieselreihe/litr/wiki/Quick-Setup).

## Development

Find more information about how to contribute in the [contribution guide](CONTRIBUTING.md).

## License

Permissive [MIT License](LICENSE).
