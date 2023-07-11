# Execution

## Built in parameters

There are a couple of built-in parameters ([reserved parameter names](/guide/parameters.html#reserved-parameter-names)) you can use with Litr.

### Help

Show the help text for your configuration with `--help` or `-h`:

```shell
litr --help
```

### Version

Show the currently installed Litr version with `--version` or `-v`:

```shell
litr --version  # 1.0.0
```

## Run command

The common case is running a defined command. This is done by using the name of the command.

```toml
# litr.toml
[commands]
hello = "echo 'Hello.'"
```

```shell
litr hello  # Hello.
```

## Multiple commands

You can also run multiple commands sequentially by using a command list with `,`.

```toml
# litr.toml
[commands]
hello = "echo 'Hello.'"
bye = "echo 'Bye.'"
```

```shell
litr hello,bye  # Hello.\nBye.
```

## Child commands

[Child commands](/guide/child-commands.html) get executed by naming the command path. An example should make this clear:

```toml
# litr.toml
[commands.build]
[commands.build.app]
[commands.build.app.tests]
script = "build test"
```

```shell
litr build app tests  # Execute the child tests command
```

This will only execute the child command `tests`. By calling a parent command you can **execute all child commands** under that parent, where the **parent is called first**. Given the following configuration:

```toml
# litr.toml
[commands.build]
script = "build preparations"

[commands.build.app]
script = "build app"

[commands.build.docs]
script = "build docs"
```

You can execute all child commands in order by running:

```shell
litr build
```

This will first run `build`, then `app` and at last `docs`. If you only want to run the child commands you can use a command list with `,`:

```shell
litr build app,docs
```

This will **not** run `build`, but only `app` and then `docs`.

## Parameters

There are two different ways to specify parameters for a command. One is for a specific command, the other is for multiple.

### Parameter for one command

To set a parameter for one command (will also apply to child commands) you write the parameter **after** the command.

```toml
# litr.toml
[commands.build]
script = "build %{target}"

[commands.update]
script = "update %{target}"

[params.target]
description = "Define a build target."
default = "release"
```

Setting the `target` parameter for the `build` command, but not the `update`. This will then call `build` with `target` "debug" and then `update` with the default "release".

```shell
litr build --target=debug , update
```

### Parameter for multiple commands

To set a parameter for multiple commands (and there child commands) you write the parameter **before** the commands.

```toml
# litr.toml
[commands.build]
script = "build %{target}"

[commands.update]
script = "update %{target}"

[params.target]
description = "Define a build target."
default = "release"
```

Setting the `target` parameter for the `build` and `update` command to "debug".

```shell
# Call build and then update with target
litr --target=debug build,update
```
