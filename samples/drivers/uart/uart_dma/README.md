# Use DMA to receive and send UART data
## Overview
***
In this example project, UART receives/sends data via DMA.

## Port Settings
***
- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

## Running the example
***
When the project runs correctly, the serial port terminal will output the following information:
```
> ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWX
Channel 0 transfers done!

```
Manually input the char through the serial port, such as 'A', and the serial port terminal will receive the following information:
```
>  Channel 1 transfers done!
A
```