---
draft: true
date: 2025-05-06T09:58:55+08:00
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

Note: This is a software implementation; Arduino also provides an [SPI library](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/) that uses the hardware implementation, which is faster but only works on specific pins.
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

Note: This is a software implementation; Arduino also provides an [SPI library](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/) that uses the hardware implementation, which is faster but only works on specific pins.
{{% /callout %}}

**Parameters**:
* `dataPin`: the pin on which to output each bit. Allowed data types: int.
* `clockPin`: the pin to toggle once the dataPin has been set to the correct value. Allowed data types: int.
* `bitOrder`: which order to shift out the bits; either `MSBFIRST` or `LSBFIRST`. (Most Significant Bit First, or, Least Significant Bit First).
* `value`: the data to shift out. Allowed data types: byte.

**Returns**: Nothing


## Interrupts

### [interrupts()](https://docs.arduino.cc/language-reference/en/functions/interrupts/interrupts/)

Re-enables interrupts (after they’ve been disabled by `noInterrupts()`) Interrupts allow certain important tasks to happen in the background and are enabled by default. Some functions will not work while interrupts are disabled, and incoming communication may be ignored. Interrupts can slightly disrupt the timing of code, however, and may be disabled for particularly critical sections of code.



### [noInterrupts()](https://docs.arduino.cc/language-reference/en/functions/interrupts/noInterrupts/)

Disables interrupts (you can re-enable them with interrupts()). Interrupts allow certain important tasks to happen in the background and are enabled by default. Some functions will not work while interrupts are disabled, and incoming communication may be ignored. Interrupts can slightly disrupt the timing of code, however, and may be disabled for particularly critical sections of code.


### [attachInterrupt(interrupt, ISR, mode)](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/)
Interrupts help make things happen automatically in microcontroller programs and can help solve timing problems. Good tasks for using an interrupt may include reading a rotary encoder, or monitoring user input.

**Parameters**:
* `interrupt`: the number of the interrupt. Allowed data types: int.
* `pin`: the Arduino pin number.
* `ISR`: the ISR to call when the interrupt occurs; this function must take no parameters and return nothing. This function is sometimes referred to as an interrupt service routine.
* `mode`: defines when the interrupt should be triggered. Four constants are predefined as valid values:
  * `LOW` to trigger the interrupt whenever the pin is low,
  * `CHANGE` to trigger the interrupt whenever the pin changes value
  * `RISING` to trigger when the pin goes from low to high,
  * `FALLING` for when the pin goes from high to low.

**Returns**: Nothing

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Interrupt Service Routines (ISR)</div>

ISRs are special kinds of functions that have unique limitations not shared by most other functions. An ISR cannot have any parameters and it should not return anything.

Generally, an ISR should be as short and fast as possible. If your sketch uses multiple ISRs, only one can run at a time; Other interrupts will be executed after the current one finishes, in an order that depends on their priority. millis() relies on interrupts to count, so it will never increment inside an ISR. Since delay() requires interrupts to work, it will not function if called inside an ISR. micros() works initially but starts behaving erratically after 1-2 ms. delayMicroseconds() does not use a counter, so it will work as usual.

Typically, global variables are used to pass data between an interrupt service routine (ISR) and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

For more information on interrupts, see [Nick Gammon’s notes](http://gammon.com.au/interrupts).

{{% /callout %}}


### [detachInterrupt(interrupt)](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/detachInterrupt/)
Turns off a given interrupt that was attached previously.

**Parameters**:
* `interrupt`: the number of the interrupt to disable (see attachInterrupt() for more details)

**Returns**: Nothing


### [digitalPinToInterrupt(pin)](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/digitalPinToInterrupt/)
To verify if a given pin can be used an interrupt.

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Digital Pins With Interrupts</div>

A full list of supported interrupt pins on all boards can be found [here](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/#digital-pins-with-interrupts).
{{% /callout %}}


**Parameters**
* `pin`: the pin we want to use for an interrupt.
  
**Returns**:
* If the pin is available for interrupt, it will return the given pin (e.g. 2).
* If the pin is not available for interrupt, it will return `-1`.


## Time

### [delay(ms)](https://docs.arduino.cc/language-reference/en/functions/time/delay/)
Pauses the program for the amount of time (in milliseconds).


**Parameters**:
* `ms`: the number of milliseconds to pause. Allowed data types: unsigned long.

**Returns**: Nothing


### [delayMicroseconds(us)](https://docs.arduino.cc/language-reference/en/functions/time/delayMicroseconds/)
Pauses the program for the amount of time (in microseconds).

**Parameters**
* `us`: the number of microseconds to pause. Allowed data types: unsigned int.

**Returns**: Nothing


### [micros()](https://docs.arduino.cc/language-reference/en/functions/time/micros/)
Returns the number of microseconds since the Arduino board began running the current program.
This number will overflow (go back to zero), after approximately **70 minutes**.


### [millis()](https://docs.arduino.cc/language-reference/en/functions/time/millis/)
Returns the number of milliseconds passed since the Arduino board began running the current program. 
This number will overflow (go back to zero), after approximately **50 days**.


## Characters, Bytes and Bits

### Characters
* [isAlpha(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isAlpha/): analyze if a char is alpha (that is a letter).
* [isAlphaNumeric(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isAlphaNumeric/): analyze if a char is alphanumeric (that is a letter or a number). 
* [isAscii(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isAscii/): analyze if a char is ASCII.
* [isControl(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isControl/): analyze if a char is a control character. 
* [isDigit(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isDigit/): analyze if a char is a digit (that is a number).
* [isGraph(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isGraph/): analyze if a char is printable with some content (space is printable but has no content).
* [isHexadecimalDigit(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isHexadecimalDigit/): analyze if a char is a hexadecimal digit (A-F, 0-9).
* [isLowerCase(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isLowerCase/): analyze if a char is lower case (that is a letter in lower case).
* [isPrintable(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isPrintable/): analyze if a char is printable (that is any character that produces an output, even a blank space).
* [isPunct(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isPunct/): analyze if a char is punctuation (that is a comma, a semicolon, an exclamation mark and so on).
* [isSpace(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isSpace/): analyze if a char is a white-space character.
* [isUpperCase(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isUpperCase/): analyze if a char is upper case (that is, a letter in upper case).
* [isWhitespace(thisChar)](https://docs.arduino.cc/language-reference/en/functions/characters/isWhitespace/): analyze if a char is a space character.

### Bytes
* [highByte(x)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/highByte/): Extracts the high-order (leftmost) byte of a word (or the second lowest byte of a larger data type).
* [lowByte(x)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/lowByte/): Extracts the low-order (rightmost) byte of a variable (e.g. a word).

### Bits

* [bit(n)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/bit/): Computes the value of the specified bit (bit 0 is 1, bit 1 is 2, bit 2 is 4, etc.).
* [bitClear(x, n)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/bitClear/): Clears (writes a 0 to) a bit of a numeric variable.
* [bitRead(x, n)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/bitRead/): Reads a bit of a variable, e.g. `bool`, `int`. Note that `float` & `double` are not supported.
* [bitSet(x, n)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/bitSet/): Sets (writes a 1 to) a bit of a numeric variable.
* [bitWrite(x, n, b)](https://docs.arduino.cc/language-reference/en/functions/bits-and-bytes/bitWrite/): Writes to a bit of a variable, e.g. `bool`, `int`, `long`. Note that `float` & `double` are not supported. 


## Math

* [abs(x)](https://docs.arduino.cc/language-reference/en/functions/math/abs/): calculates the absolute value of a given number.
* [constrain(x, a, b)](https://docs.arduino.cc/language-reference/en/functions/math/constrain/): constraints a number to be within a range. 
* [map(value, fromLow, fromHigh, toLow, toHigh)](https://docs.arduino.cc/language-reference/en/functions/math/map/): re-maps a number from one range to another.
* [max(x, y)](https://docs.arduino.cc/language-reference/en/functions/math/max/): compare two numbers and find the larger.
* [min(x, y)](https://docs.arduino.cc/language-reference/en/functions/math/min/): compare two numbers and find the smaller.
* [pow(base, exponent)](https://docs.arduino.cc/language-reference/en/functions/math/pow/): raise a number to a given power.
* [sq(x)](https://docs.arduino.cc/language-reference/en/functions/math/sq/): compute the square of a number.
* [sqrt(x)](https://docs.arduino.cc/language-reference/en/functions/math/sqrt/): find the square root of a given number.
* [cos(rad)](https://docs.arduino.cc/language-reference/en/functions/trigonometry/cos/): calculate the cosine of a given angle in radians. The result will be between -1 and 1.
* [sin(rad)](https://docs.arduino.cc/language-reference/en/functions/trigonometry/sin/): calculate the sine of a given angle in radians.The result will be between -1 and 1.
* [tan(rad)](https://docs.arduino.cc/language-reference/en/functions/trigonometry/tan/): calculate the tangent of a given angle in radians. The result will be between negative infinity and infinity.
* [random(max) / random(min, max)](https://docs.arduino.cc/language-reference/en/functions/random-numbers/random/): generates pseudo-random numbers.
* [randomSeed(seed)](https://docs.arduino.cc/language-reference/en/functions/random-numbers/randomSeed/): initializes the pseudo-random number generator, causing it to start at an arbitrary point in its random sequence.


## Communication

### [SPI](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/)
This library allows you to communicate with SPI devices, with the Arduino board as the controller device. This library is bundled with every Arduino platform (avr, megaavr, mbed, samd, sam, arc32), so you do not need to install the library separately.


**Functions**
* [SPISettings](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/SPISettings/)
* [begin()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/begin)
* [beginTransaction()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/beginTransaction)
* [endTransaction()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/endTransaction)
* [end()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/end)
* [setBitOrder()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/setBitOrder)
* [setClockDivider()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/setClockDivider)
* [setDataMode()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/setDataMode)
* [transfer()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/transfer)
* [usingInterrupt()](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/usingInterrupt)

### [Print](https://docs.arduino.cc/language-reference/en/functions/communication/print/)
The Print class is an abstract base class that provides a common interface for printing data to different output devices. It defines several methods that allow printing data in different formats.

**Functions**
* [write()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/write/)
* [print()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/print/)
* [println()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/println/)


### [Serial](https://docs.arduino.cc/language-reference/en/functions/communication/serial/)
Used for communication between the Arduino board and a computer or other devices.

**Functions**
* [if(Serial)](https://docs.arduino.cc/language-reference/en/functions/communication/serial/ifSerial/)
* [available()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/available/)
* [availableForWrite()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/availableForWrite/)
* [begin()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/)
* [end()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/end/)
* [find()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/find/)
* [findUntil()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/findUntil/)
* [flush()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/flush/)
* [parseFloat()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/parseFloat/)
* [parseInt()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/parseInt/)
* [peek()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/peek/)
* [print()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/print/)
* [println()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/println/)
* [read()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/read/)
* [readBytes()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/readBytes/)
* [readBytesUntil()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/readBytesUntil/)
* [readString()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/readString/)
* [readStringUntil()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/readStringUntil/)
* [setTimeout()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/setTimeout/)
* [write()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/write/)
* [serialEvent()](https://docs.arduino.cc/language-reference/en/functions/communication/serial/serialEvent/)


### [Stream](https://docs.arduino.cc/language-reference/en/functions/communication/stream/)
Stream is the base class for character and binary based streams. It is not called directly, but invoked whenever you use a function that relies on it.

**Functions**
* [available()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamAvailable/)
* [read()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamRead/)
* [flush()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamFlush/)
* [find()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamFind/)
* [findUntil()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamFindUntil/)
* [peek()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamPeek/)
* [readBytes()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamReadBytes/)
* [readBytesUntil()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamReadBytesUntil/)
* [readString()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamReadString/)
* [readStringUntil()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamReadStringUntil/)
* [parseInt()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamParseInt/)
* [parseFloat()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamParseFloat/)
* [setTimeout()](https://docs.arduino.cc/language-reference/en/functions/communication/stream/streamSetTimeout/)


### [Wire](https://docs.arduino.cc/language-reference/en/functions/communication/wire/)
This library allows you to communicate with I2C devices, a feature that is present on all Arduino boards. 

**Functions**
* [begin()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/begin/)
* [end()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/end/)
* [requestFrom()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/requestFrom/)
* [beginTransmission()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/beginTransmission/)
* [endTransmission()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/endTransmission/)
* [write()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/write/)
* [available()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/available/)
* [read()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/read/)
* [setClock()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/setClock/)
* [onReceive()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/onReceive/)
* [onRequest()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/onRequest/)
* [setWireTimeout()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/setWireTimeout/)
* [clearWireTimeoutFlag()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/clearWireTimeoutFlag/)
* [getWireTimeoutFlag()](https://docs.arduino.cc/language-reference/en/functions/communication/wire/getWireTimeoutFlag/)

## External Devices

### [Keyboard](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/)
The keyboard functions enable 32u4 or SAMD micro based boards to send keystrokes to an attached computer through their micro’s native USB port.


{{% callout note%}}
<div class="mt-1 font-semibold text-lg">Note</div>

Not every possible ASCII character, particularly the non-printing ones, can be sent with the Keyboard library.
{{% /callout %}}

**Functions**
* [Keyboard.begin()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardBegin/)
* [Keyboard.end()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardEnd/)
* [Keyboard.press()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardPress/)
* [Keyboard.print()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardPrint/)
* [Keyboard.println()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardPrintln/)
* [Keyboard.release()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardRelease/)
* [Keyboard.releaseAll()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardReleaseall/)
* [Keyboard.write()](https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/keyboardWrite/)

### [Mouse](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/)
The mouse functions enable 32u4 or SAMD micro based boards to control cursor movement on a connected computer through their micro’s native USB port. When updating the cursor position, it is always relative to the cursor’s previous location.

**Functions**
* [Mouse.begin()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mouseBegin/)
* [Mouse.click()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mouseClick/)
* [Mouse.end()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mouseEnd/)
* [Mouse.move()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mouseMove/)
* [Mouse.press()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mousePress/)
* [Mouse.release()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mouseRelease/)
* [Mouse.isPressed()](https://docs.arduino.cc/language-reference/en/functions/usb/Mouse/mouseIspressed/)

## [Wi-Fi](https://docs.arduino.cc/language-reference/en/functions/wifi/overview/)
The Wi-Fi API originates from the Ethernet library, and is broken out to several libraries, depending on what board and architecture used.

The API listed in the language reference is a common API that is written to accomodate all generic Wi-Fi related functions across all Wi-Fi compatible Arduino boards.


### [WiFi Network](https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass/)
The `WiFi` class contains Wi-Fi specific functions such as initializing the network settings as well as connecting to & disconnecting from a network.

* `WiFi.begin()`: Initializes the WiFi library's network settings and provides the current status.
* `WiFi.disconnect()`: Disconnects the WiFi shield from the current network.
* `WiFi.config()`: Configures a static IP address as well as change the DNS, gateway, and subnet addresses on the WiFi shield.
* `WiFi.setDNS()`: Configures the DNS (Domain Name System) server.
* `WiFi.SSID()`: Gets the SSID of the current network.
* `WiFi.BSSID()`: Gets the MAC address of the router you are connected to.
* `WiFi.RSSI()`: Gets the signal strength of the connection to the router.
* `WiFi.encryptionType()`: Gets the encryption type of the current network.
* `WiFi.scanNetworks()`: Scans for available WiFi networks and returns the discovered number.
* `WiFi.status()`: Returns the connection status.
* `WiFi.getSocket()`: Gets the first socket available.
* `WiFi.macAddress()`: Gets the MAC Address of your WiFi shield.

### [IPAddress](https://docs.arduino.cc/language-reference/en/functions/wifi/ipaddress/)
The `IPAddress` class contains methods to access the local IP address, gateway IP address and subnet mask.

* `IPAddress.localIP()`: Gets the WiFi shield's IP address.
* `IPAddress.subnetMask()`: Gets the WiFi shield's subnet mask.
* `IPAddress.gatewayIP()`: Gets the WiFi shield's gateway IP address.


### [WiFiClient](https://docs.arduino.cc/language-reference/en/functions/wifi/client/)
The `WiFiClient` class is used to connect, send and receive data to and from servers.

* `client.connected()`: Whether or not the client is connected.
* `client.connect()`: Connect to the IP address and port.
* `client.write()`: Write data to the server the client is connected to.
* `client.print()`: Print data to the server that a client is connected to.
* `client.println()`: Print data, followed by a carriage return and newline, to the server a client is connected to. 
* `client.available()`: Returns the number of bytes available for reading (that is, the amount of data that has been written to the client by the server it is connected to).
* `client.read()`: Read the next byte received from the server the client is connected to (after the last call to read()).
* `client.flush()`: Discard any bytes that have been written to the client but not yet read.
* `client.stop()`: Disconnect from the server.

### [WiFiServer](https://docs.arduino.cc/language-reference/en/functions/wifi/server/)
The `WiFiServer` is used for server based calls, such as creating server that listens to a specific port, or writing data to connected clients.

* `server.begin()`: Tells the server to begin listening for incoming connections.
* `server.available()`: Gets a client that is connected to the server and has data available for reading.
* `server.write()`: Write data to all the clients connected to a server.
* `server.print()`: Print data to all the clients connected to a server. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').
* `server.println()`: Prints data, followed by a newline, to all the clients connected to a server. 


### [WiFiUDP](https://docs.arduino.cc/language-reference/en/functions/wifi/udp/)
The `WiFiUDP` class is used send and receive UDP messages over Wi-Fi.

* `WiFiUDP.begin()`: Initializes the WiFi UDP library and network settings. Starts WiFiUDP socket, listening at local port.
* `WiFiUDP.available()`: Get the number of bytes (characters) available for reading from the buffer.
* `WiFiUDP.beginPacket()`: Starts a connection to write UDP data to the remote connection.
* `WiFiUDP.endPacket()`: Called after writing UDP data to the remote connection. It finishes off the packet and send it.
* `WiFiUDP.write()`: Writes UDP data to the remote connection. Must be wrapped between `beginPacket()` and `endPacket()`. `beginPacket()` initializes the packet of data, it is not sent until `endPacket()` is called.
* `WiFiUDP.parsePacket()`: It starts processing the next available incoming packet, checks for the presence of a UDP packet, and reports the size. parsePacket() must be called before reading the buffer with `WiFiUDP.read()`.
* `WiFiUDP.peek()`: Read a byte from the file without advancing to the next one. That is, successive calls to `peek()` will return the same value, as will the next call to `read()`.
* `WiFiUDP.read()`: Reads UDP data from the specified buffer. If no arguments are given, it will return the next character in the buffer.
* `WiFiUDP.flush()`: Discard any bytes that have been written to the client but not yet read.
* `WiFiUDP.stop()`: Disconnect from the server. Release any resource being used during the UDP session.
* `WiFiUDP.remoteIP()`: Gets the IP address of the remote connection. This function must be called after `WiFiUDP.parsePacket()`.
* `WiFiUDP.remotePort()`: Gets the port of the remote UDP connection. This function must be called after `WiFiUDP.parsePacket()`.

## Reference
* [Arduino - Language Reference](https://docs.arduino.cc/language-reference/#functions)

