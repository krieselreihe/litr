---
sidebar: auto
---

# Configuration

This document aims to define the whole specification of the Litr configuration file.

## File

The configuration file format is [TOML](https://toml.io/en/). Valid are the following file names: `litr.toml` and `.litr.toml`.

## Sections

Sections are defined as [tables](https://toml.io/en/v1.0.0-rc.2#table).

## Commands

Define commands for execution. Table keyword used is `commands`. There are two ways to define a command, a short and a detailed form.

Valid command names are defined by `[A-Za-z_][A-Za-z0-9_-]*`, meaning they start with at least one letter or underscore (`A-Za-z_`) and can further contain zero or more letters, underscore and/or hyphen characters (`A-Za-z0-9_-`). [Parameters](#parameters) can be referenced within command scripts via the name `%{name}` pattern.

### Short command form

Every valid short command is a key/value combination under the `commands` section, where **key** is the command name and **value** the command to execute as [string](https://toml.io/en/v1.0.0-rc.2#string) or [array](https://toml.io/en/v1.0.0-rc.2#array) of strings for multiple scripts. Multiple commands can be defined and mixed with [detailed commands](#detailed-command-form).

**Definition:**

```
[commands]
<key> = "<string>"
```

**Example:**

```toml
[commands]
build = "cmake ..."
run = "start ./some/script"
```

### Detailed command form

Detailed commands are defined as sub-table names with a specific set of options per command. The following options are defined:

* `script` - The actual script to run as [string](https://toml.io/en/v1.0.0-rc.2#string) or [array](https://toml.io/en/v1.0.0-rc.2#array) of strings for multiple commands
* `description` - A description that will appear when calling the help via `litr --help` as [string](https://toml.io/en/v1.0.0-rc.2#string).
* `example` - An example that will appear when calling the help via `litr --help` as [string](https://toml.io/en/v1.0.0-rc.2#string).
* `dir ` - A [string](https://toml.io/en/v1.0.0-rc.2#string) or [array](https://toml.io/en/v1.0.0-rc.2#array) of strings for multiple folders to run commands in. If defined the command will executed in all defined folders relative to the [configuration file](#file). This does not apply to child commands.
* `output` - For now only `"unchanged"` and `"silent"` are supported.

The minimum requirement for a valid, long command form, is the `script` option.

**Definition:**

```
[commands.name]
script = "<string>" | [ "<string>", ... ]
description = "<string>"
example = "<string>"
dir = "<string>" | [ "<string>", ... ]
output = "unchanged" | "silent"
```

**Example:**

```toml
[commands.build]
script = "cmake ..."
description = "Run the build script for a project"
example = """Possible
multi line string
for a usage example"""
dir = [ "src/java", "src/cpp" ]
output = "silent"
```

### Child commands

Child commands are executed either **one, directly** or **all, in order of definition**, where the first command is the "parent" command. Child commands are defined as sub-tables of a command with all options as regular commands. If a child command is defined, the parent command does not need a script to execute, but the child command does.

**Definition:**

```
[commands.parent.child]
script = "<string>" | [ "<string>", ... ]
description = "<string>"
example = "<string>"
dir = "<string>" | [ "<string>", ... ]
output = "default" | "silent"
```

**Example 1:**

```toml
[commands.build]
script = "Run first"

[commands.build.cpp]
script = "Run second"

[commands.build.java]
script = "Run third"
```

Given the first example, all three commands will run in order when called as `litr build`, meaning `build > .cpp > .java`.

**Example 2:**

```toml
[commands.build]

[commands.build.cpp]
script = "Run first"

[commands.build.java]
script = "Run second"
```

In the second example only the two child commands are run when calling `litr build` as the parent command is empty, meaning `.cpp > .java`.

**Example 3:**

```toml
[commands.build.cpp]
script = "Run first"

[commands.build.java]
script = "Run second"
```

This is the same as the second example in a shorter form. To only run **one** child command use `litr build cpp`. To run both you can use `litr build cpp,java` or just `litr build`.

## Parameters

Define names to pass values to commands. Table keyword used is `params`. Here are some rules for parameters:

* A parameter can be used in any command.
* If a parameter is used inside a command, it will appear inside the help description for that command.
* A parameter without a default will always be required for commands where its is used.
* Parameter values can be used inside command scripts by referencing the name via `%{name}`

There are two ways to define a parameters, a short and a detailed form.

Valid parameter names are defined by `[A-Za-z][A-Za-z0-9_-]*`, meaning they start with a letter (`A-Za-z`) and can after that contain zero or more letters, underscore and/or hyphen characters (`A-Za-z0-9_-`).

### Short parameter form

Every valid short parameter is a key/value combination under the `params` section, where **key** is the parameter name and **value** the parameter description as [string](https://toml.io/en/v1.0.0-rc.2#string).

**Definition:**

```
[params]
<key> = "<string>"
```

**Example:**

```toml
[params]
target = "Define a build target"
```

### Detailed parameter form

Detailed parameters are defined as sub-table name with a specific set of options per parameter. The following options are defined:

* `description` - A description that will appear when calling the help via `litr --help` as [string](https://toml.io/en/v1.0.0-rc.2#string).
* `shortcut` - A shortcut that can be used alternatively and is a single character `A-Za-z` as [string](https://toml.io/en/v1.0.0-rc.2#string).
* `type` - The default type is `"string"`. This can be an [array](https://toml.io/en/v1.0.0-rc.2#array) of strings to define possible options. Another valid value is `"boolean"`, where the default will always be `false` (therefore no default option needed).
* `default` - Define a default value for the parameter as [string](https://toml.io/en/v1.0.0-rc.2#string). This will also make the parameter **not required**.

The minimum requirement for a valid, long parameter form, is the `description` option.

**Restricted parameters:**

The following parameter names and shortcuts are restricted and will throw an error message:

* `help, h` - This parameter and it's shortcut is restricted for the built-in help text generation.
* `version, v` - This parameter and it's shortcut is restricted for the built-in version text.
* `or` - This parameter name is restricted for command script expressions.
* `and` - This parameter name is reserved, though not used anywhere, yet.

**Definition:**

```
[params.name]
description = "<string>"
shortcut = "<string>"
type = "string" | "boolean" | [ "<string>", ... ]
default = "<string>"
```

**Example:**

```toml
[params.target]
description = "Define the build target"
shortcut = "t"
type = ["debug", "release"]
default = "debug"

[params.logging]
description = "Is logging activated?"
type = "boolean"
```
