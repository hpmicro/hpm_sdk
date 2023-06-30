# SPI_SDCARD

## Overview

The example shows the use of SPI interface to read and write SD card.

## Board Setting

- [SPI PINs](lab_board_app_spi_pin) Check the information according to the board model
- The SPI pins connect the SD card(module) pins


## Running the example

confirm the hardware is correct,and the sample runs successfully, the log would be seen on the terminal like:

```console
SD Card initialization succeeded
Card Info:
-----------------------------------------------
Card Size in GBytes:    28.97GB
device block Size: 60751872 block
Block Size: 512 Bytes
sdcard wiite/read verify ....
sd block_60751852 write PASSED
sd block_60751852 read PASSED
SD write-read-verify block 0x039effec PASSED
sd block_60751853 write PASSED
sd block_60751853 read PASSED
SD write-read-verify block 0x039effed PASSED
sd block_60751854 write PASSED
sd block_60751854 read PASSED
SD write-read-verify block 0x039effee PASSED
sd block_60751855 write PASSED
sd block_60751855 read PASSED
SD write-read-verify block 0x039effef PASSED
sd block_60751856 write PASSED
sd block_60751856 read PASSED
SD write-read-verify block 0x039efff0 PASSED
sd block_60751857 write PASSED
sd block_60751857 read PASSED
SD write-read-verify block 0x039efff1 PASSED
.......
sdcard wiite/read speed test....
SD write-read-verify block range 0x039effec-0x039effff PASSED
Write Speed: 360.90KB/s, Read Speed: 3256.36KB/s
Test completed, PASSED
```