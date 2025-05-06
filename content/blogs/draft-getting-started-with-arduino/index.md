---
date: 2025-05-05T17:21:43+08:00
draft: false
title: The Language Reference
categories: 
  - Getting Started with Arduino
tags:
  - Arduino
  - Hardware 
---
## Overall Structure
Arduino programming uses a simplified form of **C++**, along with built-in functions from the **Arduino API**, making it beginner-friendly. Programs written for Arduino are called **sketches** — simple text files with a .ino extension that run on Arduino boards.

Every sketch follows a basic structure with two essential functions:
* [setup()](https://docs.arduino.cc/language-reference/en/structure/sketch/setup/): runs once when the board starts up. It’s used to initialize variables, set pin modes, and begin using libraries.
* [loop()](https://docs.arduino.cc/language-reference/en/structure/sketch/loop/): runs continuously after `setup()` finishes. This is where you write code that actively controls the board and reacts to inputs.

Together, these functions define the core flow of any Arduino program.

## Data Types and Constants

### Data Types
Arduino supports all primitive data types in C++ including `array`, `bool`, `boolean`, `byte`, `char`, `double`, `float`, `int`, `long`, `short`, `size_t`, `string`, `unsigned char`, `unsigned int`, `unsigned long`, `void`, `word`.

Conversion between the types can be done via `byte()`, `char()`, `float()`, `int()`, `long()`, `String()`, `(unsigned int)`, `(unsigned long)`, `word()`.

## Constants
### [Floating Point Constants](https://docs.arduino.cc/language-reference/en/variables/constants/floatingPointConstants/)
Floating-point constants represent decimal numbers and are used to work with non-integer values in Arduino.
* Syntax: Use a decimal point (e.g., 3.14, 0.5, -12.0)
* You can also use scientific notation with e or E to represent very large or small numbers (e.g., 1.5e2 = 150.0)

All floating-point constants are treated as **double**, but on most Arduino boards, float and double are both 4 bytes and have the same precision (6–7 digits).

### [Integer Constants](https://docs.arduino.cc/language-reference/en/variables/constants/integerConstants/)
Arduino allows you to write integer constants in different bases to represent values more clearly depending on context:
* **Decimal** (Base 10): Regular numbers (e.g., 100, -42)
* **Hexadecimal** (Base 16): Prefix with 0x or 0X (e.g., 0xFF = 255)
* **Octal** (Base 8): Prefix with 0 (e.g., 077 = 63)
* **Binary** (Base 2): Prefix with 0b or 0B (e.g., 0b1010 = 10)
  
You can use suffixes to specify the type of the integer constant:
* `U` or `u`: unsigned (e.g., 100U)
* `L` or `l`: long (e.g., 100L)
* `UL` or `ul`: unsigned long (e.g., 100UL)


### [Boolean Constants](https://docs.arduino.cc/language-reference/en/variables/constants/trueFalse/)
In Arduino, the constants `true` and `false` are used to represent boolean values:
	•	true is a constant representing the value 1.
	•	false is a constant representing the value 0.


### Arduino-specific Constants
* [HIGH | LOW](https://docs.arduino.cc/language-reference/en/variables/constants/highLow) are constants used to represent digital voltage levels. They are used with functions like `digitalWrite()`, `digitalRead()`, and `digitalReadFast()` to set or read the state of digital pins:
  * **HIGH** = 1 (typically 5V or 3.3V)
  * **LOW** = 0 (0V)
* [INPUT | INPUT_PULLUP | OUTPUT](https://docs.arduino.cc/language-reference/en/variables/constants/inputOutputPullup/) are used with the `pinMode()` function to configure how a digital pin behaves:
  * **INPUT**: Sets the pin as an input to read signals (e.g., from a button or sensor).
	* **OUTPUT**: Sets the pin as an output to send signals (e.g., turn on an LED).
	* **INPUT_PULLUP**: Sets the pin as an input and activates the internal pull-up resistor, avoiding the need for an external one. This helps prevent the pin from floating when nothing is connected.
* [LED_BUILTIN](https://docs.arduino.cc/language-reference/en/variables/constants/ledbuiltin/): is the number of the pin to which the on-board LED is connected. Most boards have this LED connected to digital pin 13.

## Qualifier

* [const](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/const/)
  The `const` qualifier in C++ is used to define variables whose values cannot be changed after initialization. In Arduino, const is often used to declare fixed configuration values like pin numbers or thresholds. It helps catch accidental changes during compilation and makes code easier to read and maintain. 
* [static](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/static/)
  The `static` qualifier serves two main purposes in C++. When used inside a function in Arduino, it causes a variable to retain its value between calls, rather than being reinitialized each time the function runs. When used at the global or class level, static restricts the variable or function’s visibility to the file or class scope, respectively. This is helpful for encapsulating internal state or limiting name conflicts in larger programs.
* [volatile](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/volatile/)
  The `volatile` qualifier tells the compiler that a variable’s value can change unexpectedly, such as through hardware or interrupt service routines. In Arduino, volatile is commonly used for variables shared between the main program and interrupt handlers. Without it, the compiler might optimize out repeated accesses, leading to incorrect behavior. Declaring a variable as volatile ensures it is always read directly from memory, preserving correct functionality in real-time or interrupt-driven contexts.


## Utilities
* [PROGMEM](https://docs.arduino.cc/language-reference/en/variables/utilities/PROGMEM/) is an Arduino-specific keyword used primarily on AVR-based boards to store constant data—such as strings or lookup tables — in **flash memory** instead of **SRAM**. This is useful for conserving limited RAM, but accessing `PROGMEM` data requires special functions like `pgm_read_byte()` or `strcpy_P()`.
* [sizeof()](https://docs.arduino.cc/language-reference/en/variables/utilities/sizeof/) is a built-in C++ operator that returns the size in bytes of a data type or variable. It is commonly used in Arduino to determine memory usage, especially when working with arrays or data structures. It operates at compile time and helps avoid hardcoding sizes.


## Scope
In C++ (and Arduino), [scope](https://docs.arduino.cc/language-reference/en/variables/variable-scope-qualifiers/scope/) defines where a variable can be accessed in the program. There are three main types of scope: **local**, **global**, and **static**, with some nuances specific to Arduino sketches.
* **Local Scope**: A variable declared inside a function, loop, or block {} is local to that block. It is created when the block starts and destroyed when it ends. You cannot access it outside its defining block.
* **Global Scope**: A variable declared outside of any function (usually at the top of your sketch) has global scope. It is accessible from any function within the same file, and lives for the entire duration of the program. Use with care to avoid conflicts and unintended side effects.
* **Static Scope**: A variable declared with the static keyword inside a function retains its value between calls, but remains accessible only within that function. It is useful for preserving state without using global variables.


## Basic Elements from C++
The following elements from C++ are supported in Arduino:
* Arithmetic Operators, `+`, `=`, `/`, `*`, `%`, `-`
* Pointer Access Operators, `*`, `&`
* Control Structure, `break`, `continue`, `do...while`, `else`, `for`, `goto`, `if`, `return`, `switch...case`, `while`
* Comparison Operators, `==`, `> `, `>=`, `< `, `<=`, `!=`
* Bitwise Operators, `<<`, `>>`, `&`, `~`, `|`, `^`
* Boolean Operators, `&&`, `!`, `||`
* Compound Operators, `+=`, `&=`, `|=`, `^=`, `/=`, `*=`, `%=`, `-=`, `--`, `++`

### Preprocessor Directives
Arduino also supports key C++ preprocessor directives such as `#define` and `#include`:

* [#define](https://docs.arduino.cc/language-reference/en/structure/further-syntax/define/): Used to assign a name to a constant value before compilation. These constants don’t consume program memory on the microcontroller—every instance is replaced with the actual value during compile time.
  **Syntax**: `#define` `CONSTANT_NAME` `value`
* [#include](https://docs.arduino.cc/language-reference/en/structure/further-syntax/include/): Used to include external libraries in your sketch.
  **Syntax**: 
  `#include <LibraryFile.h>`: searches standard library paths.
  `#include "LocalFile.h"`: searches the sketch’s directory first, then library paths if not found locally.



## Knowledge Preparation
* [C++ Programming Language](https://www.amazon.sg/dp/0321563840?ref=ppx_yo2ov_dt_b_fed_asin_title) 
* [Arduino - Language Reference](https://docs.arduino.cc/language-reference/#structure)

