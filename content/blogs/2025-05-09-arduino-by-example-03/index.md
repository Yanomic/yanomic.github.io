---
date: 2025-05-09T21:22:09+08:00
draft: false
title: Arduino by Examples - Blink
categories: 
  - Getting Started with Arduino
tags:
  - Arduino
  - Hardware 
---
This example shows the simplest thing you can do with an Arduino to see physical output: it blinks the on-board LED.

## Hardware
* Arduino UNO R4 WiFi
* LED
* 220 ohm resistor

## Circuit
This example uses the built-in LED that most Arduino boards have. This LED is connected to a digital pin and its number may vary from board type to board type. 
There is a constant, `LED_BUILTIN`, which allows you to control the built-in LED easily. 
To find out the correspondence between the constant and the digital pin, you can check the user manual of each board.
For example, in the **Pins** section of [Arduino UNO R4 WiFi](https://docs.arduino.cc/tutorials/uno-r4-wifi/cheat-sheet/), it mentions:

|Pin|	Type|	Function|
|:-----:|:-----:|:-----:|
|D13	|Digital|	SPI (SCK), GPIO pin, **Built-in LED**|


Optionally, you can also connect to an external LED with resistor.

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Why we need a resistor</div>

The resistor is essential for safe operation as it limits the current flowing through the LED, preventing damage to both the LED and the Arduino's output pin. You can choose the resistor value based on the desired current(safe current draw is usually ` 20 mA`) using Ohm's Law (`V = IR`) where `V` is the voltage of your board (5V or 3.3V) minus the forward voltage for the LED you are using (typical for red would be 1.8 to 2.2 volts).

Adjusting the resistor value allows you to control the LED's brightness while ensuring safe operation. For 5V boards you can expect the LED to be visible to a resistor value of up to 1K Ohm.

{{% /callout %}}



<div class="flex flex-row gap-3"> 
<div class="flex-none">

|Arduino Board Pin|Potentiometer Pin|
|:----:|:----:|
| D13 | Positive(Longer leg) through resistor |
| GND | Negative(Shorter leg) |


</div>
<div class="flex-">

![circuit_bb](circuit_bb.png)

</div>
</div>

### How it Works
// TODO: Describe how it works


## Code

// TODO: Add the code

// TODO: Add how to run the code

## Snapshot
// TODO: Add snapshot

## Reference
* [Blink](https://docs.arduino.cc/built-in-examples/basics/Blink/)
* [Arduino UNO R4 Wifi](https://docs.arduino.cc/hardware/uno-r4-wifi/)

