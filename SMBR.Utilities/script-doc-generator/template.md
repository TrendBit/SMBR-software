# SMBR Scripting Language Documentation

## Overview
This scripting language is designed for controlling a SMBR device capable of performing predefined actions such as lighting, measuring, and mixing. The language follows a structured indentation-based syntax similar to Python and supports basic loops and named blocks.

## Syntax
- **Indentation-based structure**: Indentation (4 spaces) is used to define blocks and nested commands.
- **Named blocks**: Each script consists of named blocks that define sequences of commands.
- **Loops**: Only `loop` cycles are supported; conditional statements (`if/else`) are not included.
- **No variables**: The language does not support variables or expressions.
- **Predefined commands**: Only specific commands are allowed with fixed parameters.

## Example Script
```plaintext
start:
    light on
    loop

main:
    for 3
        my_measure
        wait 10000

my_measure:
    illumination 0.1 0.1 0 0
    heat 10
    wait 1000
    illumination 0 0 0 0
```

## Commands

{{__COMMANDS__}}

### `light <state>`
Turns the light on or off.
- **Syntax**: `light on` or `light off`

### `wait <milliseconds>`
Pauses execution for a specified time.
- **Syntax**: `wait 1000` (waits 1000 ms)

### `mix`
Activates the mixing mechanism.
- **Syntax**: `mix`

### `measure`
Performs a measurement operation.
- **Syntax**: `measure`

### `for <count>`
Repeats the indented block of commands for the given number of iterations.
- **Syntax**:
```plaintext
for 5
    light on
    wait 10
```

### Named Blocks
Named blocks define reusable sequences of commands. They must be defined before use in the script.
- **Syntax**:
```plaintext
block_name:
    command
    command
```
- **Usage**:
  - A named block can be executed by calling its name as a command.


