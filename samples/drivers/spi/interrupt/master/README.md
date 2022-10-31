# SPI_INTERRUPT_MASTER
## Overview

The spi_interrupt_master example shows how to use spi driver as master to do board to board transfer with interrupt.
In this example, one spi instance as master and another spi instance on other board as slave. Master sends a piece of data to slave, and receives a piece of data from slave. This example checks if the data received from slave is correct.

## Board Setting

Connect [SPI Pins](lab_board_app_spi_pin) of the two boards.


## Running the example
- Run slave first, and then run master.

- When the example runs successfully, the log would be seen on the terminal like:
```
SPI-Master Interrupt Transfer Example
SPI-Master transfer timing is configured.
SPI-Master transfer source clock frequency: 24000000Hz
SPI-Master tannsfer sclk frequecny: 1562500Hz
SPI-Master transfer format is configured.
SPI master sent data:
0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 0xA6 0xA7 0xA8 0xA9
SPI master receive data:
0xB0 0xB1 0xB2 0xB3 0xB4 0xB5 0xB6 0xB7 0xB8 0xB9
SPI master transfer done.

```
