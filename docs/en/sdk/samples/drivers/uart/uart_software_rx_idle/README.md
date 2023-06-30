# UART software rx idle detection

## Overview

In this example project, UART receives variable length data with software rx idle detection via DMA(data size less than defined BUFF_SIZE in program).
This method is used when UART hardware does not support receive idle detection. Receive idle detection is realized by detecting RX signal with TRGMUX and GPTMR software.
If the hardware supports receiving idle detection, uart can use uart_hardware_rx_idle sample.

## Port Settings

- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

## Board Setting

- connect uart transmitter tx pin to uart rx pin on baord, connect GND pin between uart transmitter and baord
- connect uart rx pin to trgmux input IO

## Running the example

When the project runs correctly, send data through the UART transmitter, such as "1234567890123",  the debug serial port terminal will output the information like:
```
uart software rx idle detection.
uart receive 13 bytes, the received data are:
1 2 3 4 5 6 7 8 9 0
1 2 3

```