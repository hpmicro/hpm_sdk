# SPI_POLLING_MASTER
## Overview

The spi_polling_master example shows how to use spi driver as master to do board to board transfer with polling.
In this example, one spi instance as master and another spi instance on other board as slave. Master sends a piece of data to slave, and receives a piece of data from slave. This example checks if the data received from slave is correct.

## Board Setting

Connect [SPI Pins](lab_board_app_spi_pin) of the two boards


## Running the example

When the example runs successfully, the log would be seen on the terminal like:
```
SPI-Master Polling Transfer Example
SPI-Master transfer timing is configured.
SPI-Master transfer source clock frequency: 24000000Hz
SPI-Master tannsfer sclk frequecny: 1562500Hz
SPI-Master transfer format is configured.
SPI-Master transfer mode:write-dummy-read
SPI-Master transfer starts.
SPI-Master write command: 0x1a
SPI-Master write address:0x10
SPI-Master write data: 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9
SPI-Master read  data: 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9
SPI-Master transfer ends.
```
