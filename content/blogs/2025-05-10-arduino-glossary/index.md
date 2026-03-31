---
date: 2025-05-10T08:38:40+08:00
draft: true
title: Arduino Glossary
categories: 
  - Getting Started with Arduino
tags:
  - Arduino
  - Hardware 
---


## Hardware


### DRAM
Dynamic Random Access Memory.

Used as the main system memory (RAM) in computers and other digital systems. It is called `dynamic` because it needs to be refreshed constantly to retain data.

Slower than SRAM, but much cheaper and denser, allowing large amounts of memory.

### EEPROM
Electrically Erasable Programmable Read-Only Memory.

Storing small amounts of data that must be preserved (e.g., settings, calibration). Data retains after power is off. Slower than Flash, especially for writing.

### Flash
Flash Memory.

Long-term storage for code and data (like ROM, but rewritable). Data retains after power is off. Slower than SRAM for write/erase; reads are relatively fast.


### GPIO
General Purpose Input/Output.

GPIO pins are flexible digital signal pins on microcontrollers that can be programmed to act as either:
* **Input**, to read signals (e.g. from a button or sensor)
* **Output**, to send signals (e.g. to an LED or relay)

### PWM
Pulse Width Modulation.

Used to control analog behavior using digital signals (e.g., LED brightness, motor speed).
* Sends a square wave signal with varying duty cycles (percentage of time signal is high).
* The average voltage depends on the duty cycle, simulating an analog signal.

### SRAM
Static Random Access Memory.

Temporary storage for fast data access (used as RAM). Data is lost when power is off. Very fast read/write.


## Protocols

### I2C
Inter-Integrated Circuit.

Used for communication between multiple low-speed peripherals using just two wires.
* Uses `SCL` (clock) and `SDA` (data) lines.
* One master device controls the clock and communication; multiple slave devices can connect.
* Each device has a unique address.

### SPI
Serial Peripheral Interface.

Used for high-speed full-duplex communication between a master and one or more slaves.
* Uses four lines: MISO, MOSI, SCLK, and SS/CS.
* Master controls the clock and chip select lines.

### UART
Universal Asynchronous Receiver/Transmitter.

Used for serial communication between devices(e.g., microcontroller and computer):
* Data is transmitted bit by bit over two wires: TX (transmit) and RX (receive).
* It is asynchronous, meaning no clock signal is shared. Both devices must agree on a baud rate (e.g., 9600 bps).
