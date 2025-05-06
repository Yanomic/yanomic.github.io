---
date: 2025-05-05T17:21:43+08:00
draft: false
title: The Language Reference - Functions
categories: 
  - Getting Started with Arduino
tags:
  - Arduino
  - Hardware 
---

## Overview
The Arduino language is based on C/C++ and supports standard functions as well as Arduino-specific ones.


## I/O 


### Digital I/O

#### [pinMode(pin, mode)](https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/)
Configures the specified pin to behave either as an `INPUT` or an `OUTPUT`. 
It is possible to enable the internal pull-up resistors with the mode `INPUT_PULLUP`. Additionally, the `INPUT` mode explicitly disables the internal pull-up.

**Parameters**:
  * `pin`: the Arduino pin number to set the mode of.
  * `mode`: `INPUT`, `OUTPUT`, or `INPUT_PULLUP`. 

**Returns**: Nothing.


#### [digitalRead(pin)](https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalread/)
Reads the value from a specified digital pin, either `HIGH` or `LOW`.

**Parameters**
* `pin`: the Arduino pin number you want to read.

**Returns**: The boolean state of the read pin as `HIGH` or `LOW`.

#### [digitalWrite(pin, value)](https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalwrite/)
Write a `HIGH` or a `LOW` value to a digital pin.

* If the pin has been configured as an `OUTPUT` with `pinMode()`, its voltage will be set to the corresponding value: 5V (or 3.3V on 3.3V boards) for HIGH and 0V (ground) for LOW.
* If the pin is configured as an `INPUT`, `digitalWrite()` will enable (HIGH) or disable (LOW) the internal pull-up on the input pin. It is recommended to set the pinMode() to INPUT_PULLUP to enable the internal pull-up resistor. 
* If you do not set the pin as an OUTPUT, and connect an LED to it, when calling `digitalWrite(pin, HIGH)`, the LED may appear dim. Without explicitly setting `pinMode()`, `digitalWrite()` will have enabled the internal pull-up resistor, which acts like a large current-limiting resistor.

**Parameters**:
* `pin`: the Arduino pin number to be controlled.
* `value`: `HIGH` or `LOW`
  
**Returns**: Nothing

### Analog I/O

#### [analogRead(pin)](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/)

Reads the value from the specified analog pin.

**Parameters**:
* `pin`: the name of the analog input pin to read from.

**Returns**: The analog reading on the pin. Although it is limited to the resolution of the analog to digital converter (0-1023 for 10 bits or 0-4095 for 12 bits). Data type: int.



#### [analogWrite(pin, value)](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWrite/)

Writes an analog value ([PWM wave](https://docs.arduino.cc/learn/microcontrollers/analog-output/)) to a pin. Can be used to light a LED at varying brightnesses or drive a motor at various speeds. After a call to `analogWrite()`, the pin will generate a steady rectangular wave of the specified duty cycle until the next call to `analogWrite()` (or a call to `digitalRead()` or `digitalWrite()`) on the same pin.

**Parameters**:
* `pin`: the Arduino pin to write to. Allowed data types: int
* `value`: the duty cycle: between 0 (always off) and 255 (always on). Allowed data types: int

**Returns**: Nothing


#### [analogReadResolution(bits)](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogReadResolution/)
Sets the size (in bits) of the value returned by `analogRead()`. It defaults to 10 bits (returns values between 0-1023) for backward compatibility with AVR based boards.

You can set this between 1 and 32. You can set resolutions higher than the supported 12 or 16 bits, but values returned by analogRead() will suffer approximation. 
If you set the analogReadResolution() value to a value higher than your board’s capabilities, the Arduino board will only report back at its highest resolution, padding the extra bits with zeros.


**Parameters**:
* `bits`: determines the resolution (in bits) of the value returned by the analogRead() function. 

**Returns** : Nothing


**Parameters**:
* `type`: which type of reference to use. The options differ by types of boards.

**Returns**: Nothing


#### [analogWriteResolution(bits)](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWriteResolution/)

Sets the resolution of the `analogWrite()` function. It defaults to `8` bits (values between 0-255) for backward compatibility with AVR based boards.

**Parameters**:
* `bits`: determines the resolution (in bits) of the values used in the analogWrite() function. The value can range from 1 to 32.

**Returns**: Nothing

#### [analogReference(type)](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogReference/)
Configures the reference voltage used for analog input (i.e. the value used as the top of the input range). 

After changing the analog reference, the first few readings from `analogRead()` may not be accurate.

Don’t use anything less than 0V or more than 5V for external reference voltage on the **AREF** pin.
If you’re using an external reference on the **AREF** pin, you must set the analog reference to **EXTERNAL** before calling analogRead().



### Advanced I/O

#### [tone(pin, frequency[, duration])](https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/)
Generates a square wave of the specified frequency (and 50% duty cycle) on a pin.

**Parameters**:
* `pin`: the Arduino pin on which to generate the tone.
* `frequency`: the frequency of the tone in hertz. Allowed data types: unsigned int.
* `duration`: the duration of the tone in milliseconds (optional). Allowed data types: unsigned long.

**Returns**: Nothing


#### [noTone(pin)](https://docs.arduino.cc/language-reference/en/functions/advanced-io/noTone/)

Stops the generation of a square wave triggered by tone(). Has no effect if no tone is being generated.

**Parameters**:
* `pin`: the Arduino pin on which to stop generating the tone

**Returns**: Nothing



#### [pulseIn(pin, value[, timeout])](https://docs.arduino.cc/language-reference/en/functions/advanced-io/pulseIn/)

Reads a pulse (either `HIGH` or `LOW`) on a pin.


**Parameters**
* `pin`: the number of the Arduino pin on which you want to read the pulse. Allowed data types: int.
* `value`: type of pulse to read: either `HIGH` or `LOW`. Allowed data types: int.
* `timeout` (optional): the number of microseconds to wait for the pulse to start; default is one second. Allowed data types: unsigned long.

**Returns**: The length of the pulse (in microseconds) or 0 if no pulse started before the timeout. Data type: unsigned long.


#### [pulseInLong(pin, value[, timeout])](https://docs.arduino.cc/language-reference/en/functions/advanced-io/pulseInLong/)
Reads a pulse (either `HIGH` or `LOW`) on a pin. `pulseInLong()` is an alternative to `pulseIn()` which is better at handling long pulse and interrupt affected scenarios.

**Parameters**: 
* `pin`: the number of the Arduino pin on which you want to read the pulse. Allowed data types: int.
* `value`: type of pulse to read: either HIGH or LOW. Allowed data types: int.
* `timeout` (optional): the number of microseconds to wait for the pulse to start; default is one second. Allowed data types: unsigned long.

**Returns**: The length of the pulse (in microseconds) or 0 if no pulse started before the timeout. Data type: unsigned long.


#### [shiftIn(dataPin, clockPin, bitOrder)](https://docs.arduino.cc/language-reference/en/functions/advanced-io/shiftIn/)

Shifts in a byte of data one bit at a time. Starts from either the most (i.e. the leftmost) or least (rightmost) significant bit. 
For each bit, the clock pin is pulled high, the next bit is read from the data line, and then the clock pin is taken low.

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Note</div>

Note: This is a software implementation; Arduino also provides an SPI library that uses the hardware implementation, which is faster but only works on specific pins.
{{% /callout %}}

**Parameters**
* `dataPin`: the pin on which to input each bit. Allowed data types: int.
* `clockPin`: the pin to toggle to signal a read from dataPin.
* `bitOrder`: which order to shift in the bits; either `MSBFIRST` or `LSBFIRST`. (Most Significant Bit First, or, Least Significant Bit First).

**Returns**: The value read. Data type: byte.

#### [shiftOut(dataPin, clockPin, bitOrder, value)](https://docs.arduino.cc/language-reference/en/functions/advanced-io/shiftOut/)
Shifts out a byte of data one bit at a time. Starts from either the most (i.e. the leftmost) or least (rightmost) significant bit. 
Each bit is written in turn to a data pin, after which a clock pin is pulsed (taken high, then low) to indicate that the bit is available.

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Note</div>

Note: This is a software implementation; Arduino also provides an SPI library that uses the hardware implementation, which is faster but only works on specific pins.
{{% /callout %}}

**Parameters**:
* `dataPin`: the pin on which to output each bit. Allowed data types: int.
* `clockPin`: the pin to toggle once the dataPin has been set to the correct value. Allowed data types: int.
* `bitOrder`: which order to shift out the bits; either `MSBFIRST` or `LSBFIRST`. (Most Significant Bit First, or, Least Significant Bit First).
* `value`: the data to shift out. Allowed data types: byte.

**Returns**: Nothing


## Math

* [abs(x)](https://docs.arduino.cc/language-reference/en/functions/math/abs/): calculates the absolute value of a given number.
* [constrain(x, a, b)](https://docs.arduino.cc/language-reference/en/functions/math/constrain/): constraints a number to be within a range. 
* [map(value, fromLow, fromHigh, toLow, toHigh)](https://docs.arduino.cc/language-reference/en/functions/math/map/): re-maps a number from one range to another.
* [max(x, y)](https://docs.arduino.cc/language-reference/en/functions/math/max/): compare two numbers and find the larger.
* [min(x, y)](https://docs.arduino.cc/language-reference/en/functions/math/min/): compare two numbers and find the smaller.
* [pow(base, exponent)](https://docs.arduino.cc/language-reference/en/functions/math/pow/): raise a number to a given power.
* [sq(x)](https://docs.arduino.cc/language-reference/en/functions/math/sq/): compute the square of a number.
* [sqrt(x)](https://docs.arduino.cc/language-reference/en/functions/math/sqrt/): find the square root of a given number.

## Bits and Bytes


## Reference
* [Arduino - Language Reference](https://docs.arduino.cc/language-reference/#functions)

