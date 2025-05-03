---
date: 2025-05-03T17:54:33+08:00
draft: false
title: Getting Started with TinyGo
categories: 
  - Creative DIY Microcontroller Project with TinyGo adn WebAssembly
tags:
    - Hardware 
    - Arduino
    - TinyGo
    - WebAssembly
---
{{< spoiler text="Source Code" >}}

[creative-diy-microcontroller-project-with-tinygo-adn-webassembly/ch01/](https://github.com/Yanomic/creative-diy-microcontroller-project-with-tinygo-adn-webassembly/blob/main/ch01/)
{{< /spoiler >}}

## System Specifications
* Hardware: MacBook Pro w/ Apple M3 Pro
* System: macOS 15.4.1 (24E263)


## Why Go and TinyGo
[Go](https://go.dev/) is a statically typed, open-source programming language created by Google. It features a clean and concise syntax, built-in support for concurrency, and delivers performance comparable to C/C++. At the same time, it provides memory safety through garbage collection.

[TinyGo](https://tinygo.org/) is a new compiler for the [Go](https://go.dev/)  programming Language, designed to bring Go to embedded systems and WebAssembly. It compiles Go code into much smaller binaries and supports a broader range of platforms, making it ideal for resource-constrained environments.

## Install Go and TinyGo
```bash
# Go, https://go.dev/doc/install
brew install go
go version
# TinyGo, https://tinygo.org/getting-started/install/
brew tap tinygo-org/tools
brew install tinygo
```
The version used through out the tutorial.
```bash
$ go version
go version go1.24.0 darwin/arm64
$ tinygo version
tinygo version 0.37.0 darwin/arm64 (using go version go1.24.0 and LLVM version 19.1.2)
```

## How TinyGo Works
1. **Compiles Go Code to LLVM IR**. TinyGo takes your Go code and converts it into LLVM Intermediate Representation (IR) instead of directly compiling to machine code like the standard Go compiler.
    * It uses LLVM, a powerful compiler infrastructure, to target multiple architectures.
	* TinyGo strips away features of Go that are too memory- or CPU-heavy (like reflection, some parts of the net and os packages, etc.).
2. **Optimizes for Tiny Devices**. After generating the LLVM IR, TinyGo optimizes it for:
	* Size: to fit in microcontroller flash memory (often only a few KB or MB).
	* Speed: to run efficiently on slower CPUs (as slow as 16 MHz).
	* Memory usage: using static memory allocation where possible instead of garbage collection.
3. **Generates Machine Code for the Target**. Using the LLVM backend, it converts the optimized IR into native machine code for:
    * ARM (Cortex-M for microcontrollers)
	* RISC-V
	* WebAssembly (WASM)
	* x86 (for small desktop apps or WASM preview)
4. **Flashes or Exports the Binary**. Depending on your target, TinyGo:
	* Flashes the binary onto a microcontroller using `tinygo flash -target=<board> main.go`
	* Exports a .wasm file for web apps
	* Generates a native executable for constrained systems

## TinyGo vs. Go: A Size Comparison
Let’s start with a basic “Hello World” program in Go:
```go
package main

func main() {
	print("Hello World\n")
}
```
Now, compile it using both the standard Go compiler and TinyGo:
```bash
ch01 $ go build -o hello_world-go
ch01 $ tinygo build -o hello_world-tinygo
```
Check the size of the resulting binaries:
```bash
ch01 $ ls -alt | grep hello_world-
-rwxr-xr-x@ 1 yan  staff    55584 May  3 20:43 hello_world-tinygo
-rwxr-xr-x@ 1 yan  staff  1540818 May  3 20:43 hello_world-go
```
As you can see, the TinyGo binary is dramatically smaller—less than `4%` of the size of the binary produced by the standard Go compiler.


## Finishing the Setup
### Install Dependencies for AVR-Based Microcontrollers
To compile programs for AVR microcontrollers (like Arduino Uno), you’ll need to install a few additional tools. Run the following commands:
```bash
brew tap osx-cross/avr
brew install avr-gcc
brew install avrdude
```

### Setting Up VS Code Integration
* Go Extension
  * Open **Extensions** view and search for **Go**
  * After installation, run the `Go: Install/Update Tools` command from the Command Palette
  * Select all tools in the list and click `OK`
* TinyGo Extension
  * Open **Extensions** view and search for **TinyGo**
  * Run the command `TinyGo target` from the Command Palette
  * Choose `arduino` as the target; this will automatically generate a new `.vscode/settings.json` file in your project

## Reference
* [TinyGo](https://tinygo.org/)
* [Floating Pin Explained](https://www.youtube.com/watch?v=6U9l8U2kPfQ)