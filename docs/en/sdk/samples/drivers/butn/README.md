# BUTN

## Overview

The BUTN example shows how to use PBUTN to enter or exit power down mode.

## Note

When entering power down mode, code will be lost in RAM. To demonstrate this sample, code cannot be placed in RAM.

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, press PBUTN 16s, enter power down mode. In power down mode, press PBUTN 0.5s, exit power down mode.
In run mode, the terminal prints the fellowing message and echo received characters. In pwoer down mode, the terminal does not echo characters:
```console
Please press PBUTN 16s to enter power down mode, then press PBUTN 0.5s to wake up from power down mode.
```
