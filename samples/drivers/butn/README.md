# BUTN
## Overview

The BUTN example show how to use PBUTN and WBUTN to enter or exit power down mode.

## Note

When entering power down mode, code will be lost in RAM. To demonstrate this sample, code cannot be placed in RAM.

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, press PBUTN 16s, enter power down mode. In power down mode, press PBUTN or WBUTN 0.5s, exit power down mode.
When press PBUTN/WBUTN, PLED/WLED flashing according to the following rules.
1. when butn input high level, led off
2. when butn input low level(keep 0.5s), led flashes quickly
3. when butn input low level(keep 4s), led flashes slowly
4. when butn input low level(keep 16s), led off again

The following message is displayed in the terminal and board will send back received characters:
```
> Please press PBUTN 16s to enter power down mode, then press PBUTN or WBUTN 0.5s to wake up from power down mode.
```
