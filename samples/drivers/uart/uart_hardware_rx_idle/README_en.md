# UART hardware rx idle detection
## Overview

In this example project, UART using hardware rx idle detection function to receive variable length data via DMA(data size less than defined BUFFER_SIZE in program).

## Port Settings

- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

## Board Setting
- No special settings are required

## Running the example

When the project runs correctly, input some data by the serial port terminal, then the terminal will output the information like:
```console
uart hardware rx idle detection.
uart receive 13 bytes, the received data are:
1 2 3 4 5 6 7 8 9 0
1 2 3
```
