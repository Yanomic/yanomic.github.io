---
date: 2025-05-05T17:21:43+08:00
draft: false
title: The Language Reference - Basics
categories: 
  - Getting Started with Arduino
tags:
  - Arduino
  - Hardware 
---
## Structure
Arduino programming uses a simplified version of **C++** combined with built-in functions from the **Arduino API**, making it beginner-friendly. Programs written for Arduino are called **sketches**, which are basic text files with a `.ino` extension that run on Arduino boards.

Every sketch follows a simple structure with two key functions:
* [setup()](https://docs.arduino.cc/language-reference/en/structure/sketch/setup/): Runs once when the board starts. It is used to initialize variables, configure pin modes, and set up libraries.
* [loop()](https://docs.arduino.cc/language-reference/en/structure/sketch/loop/): Executes repeatedly after `setup()` finishes. This function contains the code that controls the board and responds to inputs.

Together, these functions form the main structure of any Arduino program.

## Data Types and Constants

### Data Types
Arduino supports many standard C++ primitive data types, such as `bool`, `byte`, `char`, `double`, `float`, `int`, `long`, `short`, `size_t`, `String`, `unsigned char`, `unsigned int`, `unsigned long`, `void`, and `word`.  
Note: In Arduino, `boolean` is treated as `bool`.

For type conversion, functions like `byte()`, `char()`, `float()`, `int()`, `long()`, `String()`, and casting such as `(int)`, `(unsigned int)`, `(unsigned long)`, and `word()` are available.

### Constants

#### [Floating Point Constants](https://docs.arduino.cc/language-reference/en/variables/constants/floatingPointConstants/)
Floating-point constants represent decimal numbers and are used to handle non-integer values in Arduino.
* Syntax: Use a decimal point (e.g., 3.14, 0.5, -12.0).
* You can also use scientific notation with `e` or `E` for large or small numbers (e.g., 1.5e2 = 150.0).

All floating-point constants are treated as **double**, but most Arduino boards store `float` and `double` as 4 bytes with the same precision (6–7 digits).

#### [Integer Constants](https://docs.arduino.cc/language-reference/en/variables/constants/integerConstants/)
Arduino supports writing integer constants in different bases to represent values clearly depending on the context:
* **Decimal** (Base 10): Regular numbers (e.g., 100, -42)
* **Hexadecimal** (Base 16): Prefix with `0x` or `0X` (e.g., `0xFF = 255`)
* **Octal** (Base 8): Prefix with `0` (e.g., `077 = 63`)
* **Binary** (Base 2): Prefix with `0b` or `0B` (e.g., `0b1010 = 10`)

Suffixes are used to specify the type of the integer constant:
* `U` or `u`: unsigned (e.g., `100U`)
* `L` or `l`: long (e.g., `100L`)
* `UL` or `ul`: unsigned long (e.g., `100UL`)

#### [Boolean Constants](https://docs.arduino.cc/language-reference/en/variables/constants/trueFalse/)
In Arduino, the constants `true` and `false` represent boolean values:
* `true` is a constant representing 1.
* `false` is a constant representing 0.

#### Arduino-Specific Constants
* [HIGH | LOW](https://docs.arduino.cc/language-reference/en/variables/constants/highLow): Constants representing digital voltage levels, used with functions like `digitalWrite()`, `digitalRead()`, and `digitalReadFast()`:
  * **HIGH** = 1 (typically 5V or 3.3V)
  * **LOW** = 0 (0V)
* [INPUT | INPUT_PULLUP | OUTPUT](https://docs.arduino.cc/language-reference/en/variables/constants/inputOutputPullup/): Used with the `pinMode()` function to set the behavior of a digital pin:
  * **INPUT**: Configures the pin as an input to read signals (e.g., from a button or sensor).
  * **OUTPUT**: Configures the pin as an output to send signals (e.g., turning on an LED).
  * **INPUT_PULLUP**: Configures the pin as an input and activates the internal pull-up resistor, which eliminates the need for an external one.
* [LED_BUILTIN](https://docs.arduino.cc/language-reference/en/variables/constants/ledbuiltin/): Refers to the pin number for the on-board LED. Most boards have it connected to digital pin 13.

## Basic Elements from C++
Arduino supports various elements from C++:
* Arithmetic Operators: `+`, `=`, `/`, `*`, `%`, `-`
* Pointer Access Operators: `*`, `&`
* Control Structures: `break`, `continue`, `do...while`, `else`, `for`, `goto`, `if`, `return`, `switch...case`, `while`
* Comparison Operators: `==`, `>`, `>=`, `<`, `<=`, `!=`
* Bitwise Operators: `<<`, `>>`, `&`, `~`, `|`, `^`
* Boolean Operators: `&&`, `!`, `||`
* Compound Operators: `+=`, `&=`, `|=`, `^=`, `/=`, `*=`, `%=`, `-=`, `--`, `++`

### Preprocessor Directives
Arduino also supports key C++ preprocessor directives like `#define` and `#include`:

* [#define](https://docs.arduino.cc/language-reference/en/structure/further-syntax/define/): Used to assign a constant name before compilation. These constants are replaced by their values during compilation without consuming program memory.
  **Syntax**: `#define CONSTANT_NAME value`
* [#include](https://docs.arduino.cc/language-reference/en/structure/further-syntax/include/): Includes external libraries in your sketch.
  **Syntax**: 
  `#include <LibraryFile.h>`: Searches the standard library paths.
  `#include "LocalFile.h"`: Searches the sketch’s directory first and then library paths if not found locally.

### Qualifiers

* [const](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/const/): 
  The `const` qualifier is used to define variables whose values cannot be changed after initialization. In Arduino, `const` is often used for fixed configuration values like pin numbers or thresholds. It prevents accidental changes during compilation, making the code more readable and maintainable.
* [static](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/static/):
  The `static` qualifier serves two purposes. When used inside a function, it retains a variable’s value between function calls. When used at the global or class level, it restricts the visibility of the variable or function to the file or class scope, helping to encapsulate internal state and avoid name conflicts.
* [volatile](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/volatile/): 
  The `volatile` qualifier tells the compiler that a variable can change unexpectedly, such as through hardware or interrupts. In Arduino, it's used for variables shared between the main program and interrupt service routines. Without it, the compiler might optimize out repeated accesses, leading to incorrect behavior. Declaring a variable as `volatile` ensures it is always read directly from memory.

### Utilities

* [PROGMEM](https://docs.arduino.cc/language-reference/en/variables/utilities/PROGMEM/): An Arduino-specific keyword that stores constant data (like strings or lookup tables) in **flash memory** instead of **SRAM**. This is useful for saving limited RAM, but accessing `PROGMEM` data requires special functions like `pgm_read_byte()` or `strcpy_P()`.
* [sizeof()](https://docs.arduino.cc/language-reference/en/variables/utilities/sizeof/): A C++ operator that returns the size in bytes of a data type or variable. It helps in determining memory usage, particularly when working with arrays or data structures. The size is determined at compile time, avoiding the need for hardcoded sizes.

### Scope
In C++ (and Arduino), [scope](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/scope/) defines the accessibility of a variable within the program. There are three main types of scope: **local**, **global**, and **static**, each with some nuances in Arduino sketches.
* **Local Scope**: Variables declared inside a function, loop, or block `{}` are local to that block. They are created when the block starts and destroyed when it ends. These variables cannot be accessed outside the block.
* **Global Scope**: Variables declared outside any function (usually at the top of the sketch) have global scope. They are accessible from any function within the same file and live throughout the program’s execution. Use caution to avoid conflicts and unintended side effects.
* **Static Scope**: Variables declared with the `static` keyword inside a function retain their values between calls, but are only accessible within that function. This is useful for preserving state without needing global variables.

## Reference
* [C++ Programming Language](https://www.amazon.sg/dp/0321563840?ref=ppx_yo2ov_dt_b_fed_asin_title) 
* [Arduino - Language Reference](https://docs.arduino.cc/language-reference/#structure)

