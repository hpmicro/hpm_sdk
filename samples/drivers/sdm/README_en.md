# SDM

## Overview

The SDM example shows data filter and comparator functions.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Different hardware setting are used depending on whether an AD sampling chip is on the development board.
- If there is no onboard AD sampling chip on the development board, please using an external sampling chip and connect the output CLK and DAT pins of the sampling chip to the CLK and DAT pins of the SDM on the board.
- If there is onboard AD sampling chip, simply connect the analog signal to the input interface of the sampling chip.


## Running the example

Running the example, the example will prints the received data in the terminal, Use these data to plot and compare with the original analog waveform:
```console
printf sdm received data.
10778477
13348519
14046041
14695587
15260419
15795965
16313123
16504057
15999335
15261047
14505145
14053119
13520479
```
