# Child commands

Commands can have child commands (cf. _nested commands_, _sub commands_). If not called directly, they will be [executed with the parent command](#run-child-command), in [order of definition](#order-of-execution). All [available command options](/guide/configure-commands.html) are also available for child commands.

## Define child commands

As an example here the definition of two child commands, `build.app` and `build.docs`.

```toml
# litr.toml
[commands.build.app]
script = "..."

[commands.build.docs]
script = "..."
```

In this case the parent command `build` is empty. Child commands can be as deeply nested as you wish, e.g.:

```toml
# litr.toml
[commands.build.app.tests.native]
script = "..."
```

## Run child command

Given the [earlier example](#define-child-commands) of the child commands `build.app` and `build.docs`, this is how you can run them:

```shell
# Only execute build.app
litr build app

# Only execute build.docs
litr build docs

# Run them all
litr build
```

## Order of execution

Child commands get execute in order of definition inside the configuration file. Parent commands are always executed first. Looking at the following example configuration:

```toml
# litr.toml
[commands.build]
script = "..."

[commands.build.app]
script = "..."

[commands.build.docs]
script = "..."
```

### Run all

You can run all commands, where the parent command `build` is first, `app` second and `docs` last:

```shell
litr build
```

### Run one

You can run just a specific child command:

```shell
litr build docs
```

### Run multiple

Or, you can also run multiple **without executing the parent command**. This is done by defining a list of commands via `,`.

```shell
litr build app,docs
```
