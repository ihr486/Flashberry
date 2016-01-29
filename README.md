# Flashberry

Flashberry is an RL78 programmer for the Raspberry Pi.
Most of the RL78 microcontrollers (currently excluding G10 series) can be programmed
through the embedded UART(UART0) and just one GPIO pin.

## Programming circuit

In order to use the programmer, connect the pins as follows:

| Raspberry Pi | RL78 |
|---|---|
| TXD0 | TOOLRXD and TOOL0(pullup required) |
| RXD0 | TOOLTXD |
| GPIOx | RESET(pullup required) |

Alternatively, single-wire mode (please refer to the official documentation for detail) is available.

## Input format

Currently, the program accepts Intel HEX format only.
Option bytes must be embedded into the ROM image.

## Limitations

Baudrate is fixed to 115200 due to hardware limitations.
