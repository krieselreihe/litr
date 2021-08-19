# Quick Setup
## Installation
Install Litr on depending on you operating system:

<code-group>
<code-block title="macOS">
```shell
brew install litr
```
</code-block>

<code-block title="Linux">
```shell
echo "How to install?"
```
</code-block>
</code-group>

Check the installation by showing the currently installed version:

```shell
litr --version  # 1.0.0
```

## Setup
Create a `litr.toml` file (you can also use `.litr.toml`) inside your project, user directory or whatever directory you want to execute commands from. You can read up more [about TOML on the official website](https://toml.io/).

Define your first command and parameter:

```toml
[commands]
hello = "echo Hello %{name}"

[params]
name = "Say hello to this name."
```

## Run commands
Run your command with the defined parameter:

```shell
litr hello --name=Perrin  # Hello Perrin
```

Or print the help for your file via `litr --help`:

```
Litr - Language Independent Task Runner [version 1.0.0]
  Configuration file found under: /Path/to/litr.toml

Usage: litr command [options]

Commands:
  hello  --name=<option>

Options:
  -h --help  Show this screen.
     --name  Say hello to this name.
```
