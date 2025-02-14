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

{{__COMMANDS__}}

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


