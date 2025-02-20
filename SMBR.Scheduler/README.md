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
main:
    print "testing script"
    
    for 3
        print "next loop"
        my_measure
        wait 10000

    print "testing script done"

my_measure:
    illumination 0.1 0.1 0 0
    heat 10
    wait 1000
    illumination 0 0 0 0
```

## Commands

### `display <message>`
Display message on OLED unit of the device
#### Arguments
| Name | Description | Required |
|------|-------------|----------|
| `message` | Message to display | Yes |

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



### Named Blocks
Named blocks define reusable sequences of commands. They can be called from any other place in script (infinite recursion is not checked)
- **Syntax**:
```plaintext
block_name:
    command
    command
```
- **Usage**:
  - A named block can be executed by calling its name as a command.

### Entry point
There must be named block 'main' which is entry point for script  



