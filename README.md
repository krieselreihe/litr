# litr

Litr (_Language Independent Task Runner_) lets you configure and then run any tasks you want for any language. Define scripts, run executables or execute commands on your machine. Litr serves as an abstraction layer to combine different strategies into one tool.

**Note:** This repository only serves for defining the tool, for now. Implementation will follow. Have a look at the [wiki](https://github.com/krieselreihe/litr/wiki) for more details on the current progress.

## Vision

Here is the vision of the tool and how it should work in a theoretical "quick setup" guide. **This does not yet work.**

Install the tool:

```shell script
brew install litr
```

Define some commands, to make it feel real the example wraps this project with C++ and CMake:

```toml
# litr.toml
[commands]
update = [
  "git pull",
  "git submodule update --init"
]
build = "cmake -GNinja -DCMAKE_BUILD_TYPE=${target} --build build/${target}"
run = "./build/${target}/litr/client/Client"

[params]
target = "Defines the build target"
```

Run them all:

```shell script
litr --target=debug update,build,run
```

To find out more go to the [wikis setup section](https://github.com/krieselreihe/litr/wiki/Setup).

## Development

Find more information about how to contribute in the [contribution guide](CONTRIBUTING.md).
