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

### `air <speed>`
Aerator command
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `speed` | Speed of the aerator | Yes |

### `pump <speed>`
Pump command
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `speed` | Speed of the pump | Yes |

### `heat <temperature>`
Heating command
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `temperature` | Temperature to heat to | Yes |

### `mix <time>`
Mixing command
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `time` | Time to mix | Yes |

### `illumination <i0> <i1> <i2> <i3>`
Illumination command
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `i0` | Intensity of channel 0 | Yes |
| `i1` | Intensity of channel 1 | Yes |
| `i2` | Intensity of channel 2 | Yes |
| `i3` | Intensity of channel 3 | Yes |

### `print <content>`
Print text to script output.
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `content` | Content to print (any arguments on the line will be concatenated) | Yes |

### `wait <milliseconds>`
Pauses execution for a specified time.
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `milliseconds` | Time to wait in milliseconds | Yes |

### `loop [times]`
Repeats the indented block of commands for the given number of iterations.
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `times` | Number of times to loop - infinity if not specified | No |



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



