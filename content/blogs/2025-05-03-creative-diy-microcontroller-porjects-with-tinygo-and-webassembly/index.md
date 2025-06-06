---
date: 2025-05-03T17:54:33+08:00
draft: true
title: Getting Started with TinyGo
categories: 
  - Creative DIY Microcontroller Project with TinyGo adn WebAssembly
tags:
    - Hardware 
    - Arduino
    - TinyGo
    - WebAssembly
---



{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Limitation of TinyGo on Arduino</div>

TinyGo is currently not suitable for projects requiring:
* Wireless communication (Wi-Fi, Bluetooth)
* USB peripherals beyond basic serial
* Filesystems or media (SD cards, audio, video)
* Networking or IoT protocols (MQTT, HTTP)
* Complex sensor stacks or 3rd-party hardware libraries

### TinyGo Feature Support on Arduino Boards

| Feature                           | TinyGo Support                         | C++ (Arduino) Support                    | Notes                                                 |
|-----------------------------------|----------------------------------------|------------------------------------------|-------------------------------------------------------|
| **Wi-Fi (e.g., ESP32, Nano 33 IoT)** | ⚠️ Partial support via `nina-fw` firmware | ✅ Full support via libraries (WiFiNINA, ESP32 WiFi) | Requires specific firmware; limited functionality |
| **Bluetooth (BLE)**              | ✅ Supported on select boards           | ✅ Supported on boards like Nano 33 BLE  | Requires `nina-fw` firmware and supported hardware |
| **USB (as host or device)**      | ⚠️ Basic serial USB only               | ✅ Full USB HID, MIDI, mass storage, etc. | HID, keyboard/mouse emulation not supported          |
| **Networking Libraries**         | ⚠️ Limited support                     | ✅ Wide variety: MQTT, HTTP, WebSockets   | No `net/http` or TCP stack in TinyGo                 |
| **OTA (Over-the-Air updates)**   | ❌ Not supported                        | ✅ Available on many WiFi-capable boards  | Crucial for remote firmware updates                  |
| **Audio / DAC / I2S**            | ❌ Not supported                        | ✅ Fully supported (e.g., PCM, WAV playback) | No support for audio interfaces                  |
| **Camera / Display Drivers**     | ⚠️ Limited support                     | ✅ Extensive (TFT, OLED, camera modules)  | Limited driver availability in TinyGo                |
| **File System (SD, SPIFFS)**     | ✅ Supported via TinyFS                 | ✅ Full support for SD cards, flash FS    | Requires additional setup and supported hardware     |
| **Sensor Libraries**             | ✅ Supported via drivers                | ✅ Thousands available (e.g., MPU6050)     | Growing collection of drivers in TinyGo              |
| **Interrupt Priorities / Nested IRQs** | ⚠️ Basic support                  | ✅ Supported via register control         | Limited configurability in TinyGo                    |
| **Real-Time OS (RTOS)**          | ❌ Not supported                        | ✅ Available via FreeRTOS, ChibiOS        | No threading, mutexes, or RTOS APIs                 |


{{% /callout %}}


{{< spoiler text="Source Code" >}}

[creative-diy-microcontroller-project-with-tinygo-adn-webassembly/ch01/](https://github.com/Yanomic/creative-diy-microcontroller-project-with-tinygo-adn-webassembly/blob/main/ch01/)
{{< /spoiler >}}
## System Specifications
* Hardware: MacBook Pro with Apple M3 Pro
* System: macOS 15.4.1


## Why Go and TinyGo
[Go](https://go.dev/) is a statically typed, open-source programming language created by Google. It features a clean and concise syntax, built-in support for concurrency, and delivers performance comparable to C/C++. At the same time, it provides memory safety through garbage collection.

[TinyGo](https://tinygo.org/) is a new compiler for the [Go](https://go.dev/)  programming Language, designed to bring Go to embedded systems and WebAssembly. It compiles Go code into much smaller binaries and supports a broader range of platforms, making it ideal for resource-constrained environments.

## Install Go and TinyGo
Use the following commands to install Go and TinyGo on macOS:
```bash
# Go, https://go.dev/doc/install
brew install go
go version
# TinyGo, https://tinygo.org/getting-started/install/
brew tap tinygo-org/tools
brew install tinygo
```
Here are the versions used throughout this tutorial:
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
### Install Arduino IDE
* Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
* Launch the IDE and navigate to **Tools > Board > Board Manager**
* In the search bar, enter `uno` or `uno r4`, then locate:
	* **Arduino AVR Boards** (for Uno R3 and similar), or
	* **Arduino Renesas UNO R4 Boards** (for Uno R4 Minima and WiFi)
* Click Install next to the appropriate package.
* After installation, go to Tools > Board and select the board you’re using.

### Install Dependencies for AVR-Based Microcontrollers
To compile programs for AVR microcontrollers (like Arduino Uno R3), you’ll need to install a few additional tools. Run the following commands:
```bash
brew tap osx-cross/avr
brew install avr-gcc
brew install avrdude
```
### Install Dependencies for ARM-Based Microcontrollers
To compile and upload programs to ARM-based boards such as the Arduino Uno R4, install the ARM GCC toolchain. Run the following command:
```bash
brew install arm-none-eabi-gcc
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