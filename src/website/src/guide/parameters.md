# Parameters

To use variable values inside commands you can use parameters. Here are some **general rules** for parameters:

* Parameters are **global**, meaning they can be used in any command.
* Parameters are unique, so every name can only appear once.
* If a parameter is used inside a command, it will appear inside the help description for that command.
* A parameter without a [default](#defaults) will always be required for commands where its is used.
* Parameter values can be [used](#usage) inside command scripts by referencing the name via `%{name}`.

As a **small example**, this is how the parameter usage looks like:

```toml
# litr.toml
[commands]
hello = "echo Hello %{name}"

[params]
name = "Say hello to this name."
```

Running the command with the defined parameter:

```shell
litr hello --name=Perrin  # Hello Perrin
```

## Parameter names

Valid parameter names are defined by `[A-Za-z_][A-Za-z0-9_-]*`, meaning they start with at least one letter or underscore `(A-Za-z_)` and can further contain zero or more letters, underscore and/or hyphen characters `(A-Za-z0-9_-)`.

Here some examples of valid parameter names:

```toml
# litr.toml
[params]
target = "..."
enhance2Times = "..."
Debug = "..."
some-param = "..."
b2 = "..."
_42 = "..."
```

## Short parameter form

The _"short parameter form"_ is a key/value combinations under the params section, where key is the parameter name and value the parameter description.

```toml
# litr.toml
[params]
target = "Define a build target."
```

Running `litr --help` will show this description under the "Options" section of the help text:

```
...

Options:
  -h --help     Show this screen.
  -v --version  Show current Litr version.
     --target   Define a build target.
```

## Long parameter form

The _"long parameter form"_ gives you more options for defining parameters. The minimum requirement is the `description` property. This is how it looks like:

```toml
# litr.toml
[params.target]
description = "Define a build target."
```

Using this parameter would look like this:

```shell
litr --target=Something
```

## Shortcuts

You can define a shortcut for a parameter name that can be `[A-Za-z]`:

```toml
# litr.toml
[params.target]
description = "Define a build target."
shortcut = "t"
```

Usage is:

```shell
litr -t=Something
```

## Types

The default type of every parameter is `string`. Though there are two more types available; a specific list of strings (string array) and `boolean`.

### String

In general a type is defined via the `type` property.

```toml
# litr.toml
[params.target]
description = "Define a build target."
type = "string"  # Default value
```

### String array

You can define a specific list of strings that can be used as parameter values.

```toml
# litr.toml
[params.target]
description = "Define a build target."
type = ["debug", "release"]
```

If you use a value that is **not defined** in the list an error will be shown:

```shell
litr --target=none
```

```
Error: Parameter value "none" is no valid option for "target".
  Available options are: "debug", "release"
```

### Boolean

Another type is `boolean`, where the [default value](#defaults) is `false`.

```toml
# litr.toml
[params.profile]
description = "Run the application profiler."
type = "boolean"
```

The parameter will be set to `true` if it is defined on calling a command:

```shell
litr --profile  # profile is now true
```

You can also set the boolean value explicitly:

```shell
litr --profile=true
litr --profile=false
```

## Defaults

If a parameter **is not of type** `boolean` it **will be required**. You can change this by defining a default value for the parameter with the `default` property.

```toml
# litr.toml
[params.target]
description = "Define a build target."
type = ["debug", "release"]
default = "debug"
```

## Reserved parameter names

Some parameter names are already in use by Litr, or are otherwise reserved.

* `help`, `h` - This parameter and it's shortcut is restricted for the built-in help text generation.
* `version`, `v` - This parameter and it's shortcut is restricted for the built-in version text.
* `or` - This parameter name is restricted for command script expressions.
* `and` - This parameter name is reserved, though not used anywhere, yet.

**Disclaimer:** The names `or` and `and` may **not** be reserved in the future [[#59](https://github.com/krieselreihe/litr/issues/59)].

## Usage

Parameters can be used inside command scripts with the string interpolation pattern `%{}`.

### Inserting value

For a [string type](#types) parameter this could look like this:

```toml
# litr.toml
[commands.hello]
script = "echo Hello %{name}"

[params.name]
description = "Say my name."
```

As there is no [default value](#defaults) defined you have to provide a `name`:

```shell
litr hello --name=Mat  # Hello Mat
```

### Conditions

With a [boolean parameter](#types) you can use conditions. There is the `if`-condition:

```toml
# litr.toml
[commands.question]
script = "echo Are you ok? %{answer 'Yes.'}"

[params.ok]
description = "Are you ok?"
type = "boolean"
```

This will only print `Yes` if the parameter `ok` is given:

```shell
litr question       # Are you ok?
litr question --ok  # Are you ok? Yes.
```

Or you use the `if..else`-condition with the `or` keyword:

```toml
# litr.toml
[commands.question]
script = "echo Are you ok? %{answer 'Yes.' or 'No.'}"

[params.ok]
description = "Are you ok?"
type = "boolean"
```

```shell
litr question       # Are you ok? No.
litr question --ok  # Are you ok? Yes.
```
