# UART_DMA receive variable length data
## Overview
***
In this example project, UART receives variable length data via DMA(data size less than defined BUFFER_SIZE in program).

## Port Settings
***
- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

## Board Setting
- connect uart transmitter tx pin to uart rx pin on baord, connect GND pin between uart transmitter and baord
- connect uart rx pin to trgmux input IO

## Running the example
***
When the project runs correctly, the serial port terminal will output the following information:
```
> uart receive xxx bytes, the received data are:!

```
the received data will be seen like:
```
0 1 2 3 4 5 6 7 8 9
0 1 2 3 4 5 6 7 8 9
0 1 2 3 4 5 6 7 8 9
0 1 2 3 4 5 6 7 8 9

```