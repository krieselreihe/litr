---
sidebar: auto
---

# FAQ

## How to split a long script

Besides, using an array for multiple commands there is also a way to split long scripts into multiple lines by using the backslash `\` and the [TOML multi-line string](https://toml.io/en/v1.0.0#string).

```toml
# litr.toml
[commands.build]
script = """echo 'First line' &&\
  echo 'Second line' &&\
  echo 'Last line'"""
```
