# Flashberry

Flashberry is an RL78 programmer for the Raspberry Pi.
Most of the RL78 microcontrollers (currently excluding G10 series) can be programmed
through the embedded UART(UART0) and just one GPIO pin.

## Why RL78?

RL78 is superior to PIC16 and ATmega etc. in that it has more accurate internal OSCs,
and that they can utilize the UART for programming.
You do not need any external programmers to use these microcontrollers together with the Raspberry Pi,
which makes them ideal for original HAT development, analog signal processing, and real-time control.

## Programming circuit

In order to use the programmer, connect the pins as follows:

| Raspberry Pi | RL78 |
|---|---|
| TXD0 | TOOLRXD and TOOL0(pullup required) |
| RXD0 | TOOLTXD |
| GPIOx | RESET(pullup required) |

The default setting specifies GPIO4 for RESET pin; if you need to use another pin, reconfigure the pin number in flashberry.h and rebuild.
Alternatively, single-wire mode (please refer to the official documentation for detail) is available.

## Input format

Currently, the program accepts Intel HEX format only.
Option bytes must be embedded into the ROM image.

## Limitations

Baudrate is fixed to 115200 due to hardware limitations.
