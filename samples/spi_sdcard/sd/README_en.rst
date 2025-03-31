.. _spi_sdcard:

SPI_SDCARD
====================

Overview
--------

The example shows the use of SPI interface to read and write SD card.

Board Setting
-------------

-  :ref:`SPI PINs <board_resource>`  Check the information according to the board model

- The SPI pins connect the SD card(module) pins

Notes
-----

- This example depends on the SPI component and DMA manager component, which need to be enabled.

- Considering the compatibility of various SD cards, some SD cards need to perform write wait delay in the write operation of the SPI sdcard library. You can refer to common/adapt/spi_sd_adapt.c when porting. You need to pay attention to the following three points:

- 1. The internal delay defaults to 100us. If you need to change other delays, you can redefine the SPI_SD_DELAY_DEFAULT_US macro

- 2. If you do not need the write wait delay, you can set the delay_ms and delay_us structure members of the sdcard_spi_init API to NULL. Or the SPI_SD_DELAY_DEFAULT_US macro is defined as 0

- 3. If the write wait delay is required, and in an RTOS environment, it is recommended that the sdcard_spi_init initialization API be called in the task, and the structure members delay_ms and delay_us passed as parameters are assigned to the RTOS task delay API, such as vTaskDelay of freertos

Running the example
-------------------

confirm the hardware is correct,and the sample runs successfully, the log would be seen on the terminal like:


.. code-block:: console

   [spi_sdcard] SPI CLK frequency:400000 Hz
   [spi_sdcard] SPI CLK frequency:40000000 Hz
   SD Card initialization succeeded
   Card Info:
   -----------------------------------------------
   Card Size in GBytes:    14.84GB
   Block count: 31116288 block
   Block Size: 512 Bytes

   -----------------------------------------------
   sdcard wiite/read verify ....
   sd block_31116268 write PASSED
   sd block_31116268 read PASSED
   SD write-read-verify block 0x01dacbec PASSED
   sd block_31116269 write PASSED
   sd block_31116269 read PASSED
   SD write-read-verify block 0x01dacbed PASSED
   sd block_31116270 write PASSED
   sd block_31116270 read PASSED
   SD write-read-verify block 0x01dacbee PASSED
   sd block_31116271 write PASSED
   sd block_31116271 read PASSED
   SD write-read-verify block 0x01dacbef PASSED
   sd block_31116272 write PASSED
   sd block_31116272 read PASSED
   SD write-read-verify block 0x01dacbf0 PASSED
   sd block_31116273 write PASSED
   sd block_31116273 read PASSED
   SD write-read-verify block 0x01dacbf1 PASSED
   sd block_31116274 write PASSED
   sd block_31116274 read PASSED
   SD write-read-verify block 0x01dacbf2 PASSED
   sd block_31116275 write PASSED
   sd block_31116275 read PASSED
   SD write-read-verify block 0x01dacbf3 PASSED
   sd block_31116276 write PASSED
   sd block_31116276 read PASSED
   SD write-read-verify block 0x01dacbf4 PASSED
   sd block_31116277 write PASSED
   sd block_31116277 read PASSED
   SD write-read-verify block 0x01dacbf5 PASSED
   sd block_31116278 write PASSED
   sd block_31116278 read PASSED
   SD write-read-verify block 0x01dacbf6 PASSED
   sd block_31116279 write PASSED
   sd block_31116279 read PASSED
   SD write-read-verify block 0x01dacbf7 PASSED
   sd block_31116280 write PASSED
   sd block_31116280 read PASSED
   SD write-read-verify block 0x01dacbf8 PASSED
   sd block_31116281 write PASSED
   sd block_31116281 read PASSED
   SD write-read-verify block 0x01dacbf9 PASSED
   sd block_31116282 write PASSED
   sd block_31116282 read PASSED
   SD write-read-verify block 0x01dacbfa PASSED
   sd block_31116283 write PASSED
   sd block_31116283 read PASSED
   SD write-read-verify block 0x01dacbfb PASSED
   sd block_31116284 write PASSED
   sd block_31116284 read PASSED
   SD write-read-verify block 0x01dacbfc PASSED
   sd block_31116285 write PASSED
   sd block_31116285 read PASSED
   SD write-read-verify block 0x01dacbfd PASSED
   sd block_31116286 write PASSED
   sd block_31116286 read PASSED
   SD write-read-verify block 0x01dacbfe PASSED

   SD write-read-verify reach the total count:19  PASSED!!!!

   sdcard wiite/read speed test....
   SD write-read-verify block range 0x01dacbec-0x01dacbff PASSED
   Write Speed: 2532.18KB/s, Read Speed: 3021.68KB/s
   Test completed, PASSED
