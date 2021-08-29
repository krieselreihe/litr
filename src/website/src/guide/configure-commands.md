# Configure commands

## Command names

Valid command names are defined by `[A-Za-z_][A-Za-z0-9_-]*`, meaning they start with at least one letter or underscore `(A-Za-z_)` and can further contain zero or more letters, underscore and/or hyphen characters `(A-Za-z0-9_-)`.

Here an example of some valid command names:

```toml
# litr.toml
[commands]
hello = "..."
exec10Times = "..."
Run = "..."
good-command = "..."
a1 = "..."
_42 = "..."
```

## Short command form

So far you only used the _"short form"_ for command definitions. Those are command names under the `[commands]` block:

```toml
# litr.toml
[commands]
hello = "..."
```

That then can be called like so:

```shell
litr hello
```

See the next section for the _"long form"_ of commands with more options.

## Long command form

The _"long form"_ of commands is defined like this, where the `script` property is required:

```toml
# litr.toml
[commands.hello]
script = "..."
```

The `script` property now contains the actual script to execute, where the name is defined via `[commands.hello]`. This way you can add more options to a command, like a description via the `description` property.

## Multi-line commands

The `script` property can also take an array of strings if you want to execute multiple strings in a sequence.

```toml
# litr.toml
[commands.hello]
script = [
  "echo 'First line'",
  "echo 'Second line'"
]
```

## Command description

```toml
# litr.toml
[commands.hello]
script = "..."
description = "Say hello."
```

Showing the help via `litr --help` will print the following to your terminal:

```
Litr - Language Independent Task Runner [version 1.0.0]
  Configuration file found under: /Path/to/litr.toml

Usage: litr command [options]

Commands:
  hello         Say hello.

Options:
  -h --help     Show this screen.
  -v --version  Show current Litr version.
```

## Example text for help

For more complex commands you can also add an example text to the printed help message via the `example` property:

```toml
# litr.toml
[commands.hello]
script = "..."
description = "Say hello."
example = "litr hello"
```

You can even use [TOML multi-line strings](https://toml.io/en/v1.0.0#string):

```toml
# litr.toml
[commands.hello]
script = "..."
description = "Say hello."
example = """litr hello
litr hello --name=Martin
"""
```

Showing the help will give you:

```
Litr - Language Independent Task Runner [version 1.0.0]
  Configuration file found under: /Path/to/litr.toml

Usage: litr command [options]

Commands:
  hello         Say hello.
                ┌ Example(s):
                │ litr hello
                └ litr hello --name=Martin

Options:
  -h --help     Show this screen.
  -v --version  Show current Litr version.
```

## Running inside a directory

With the `dir` property you can run commands inside a specific directory, relative to the configuration file.

```toml
# litr.toml
[commands.hello]
script = "ls"
dir = "some/folder"
```

You can also specify an array of folders, **this will execute the command in all given folders**:

```toml
# litr.toml
[commands.hello]
script = "ls"
dir = [
  "first/folder",
  "second/folder"
]
```

## Suppress output

There is also the option to not print any output from a command you run. This is done by setting the `output` property to `"silent"`:

```toml
# litr.toml
[commands.hello]
script = "..."
output = "silent"
```
