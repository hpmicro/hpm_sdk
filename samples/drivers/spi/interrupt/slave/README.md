# SPI_INTERRUPT_SLAVEINTERRUPT
## Overview

The spi_interrupt_slave example shows how to use spi driver as slave to do board to board transfer with interrupt.
In this example, one spi instance as master and another spi instance on other board as slave. Master sends a piece of data to slave, and receives a piece of data from slave. This example checks if the data received from slave is correct.

## Board Setting

Connect [SPI Pins](lab_board_app_spi_pin) of the two boards.


## Running the example
- Run slave first, and then run master.

- When the example runs successfully, the log would be seen on the terminal like:
```
SPI-Slave Interrupt Transfer Example
SPI-Slave transfer format is configured.
SPI-Slave transfer waits.
SPI slave receive command: 0x1A
SPI slave receive data:
0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 0xA6 0xA7 0xA8 0xA9
SPI slave sent data:
0xB0 0xB1 0xB2 0xB3 0xB4 0xB5 0xB6 0xB7 0xB8 0xB9
SPI slave transfer done.
```