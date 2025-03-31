.. _spi_sdcard_fatfs:

SPI SDCard FATFS
================================

Overview
--------

the sample use the SPI interface to operate SD card and use the FATFS file system, the sample needs to format the SD card as FAT32, The sample provides the following features:

- Create hello.txt

- Read 1st line from hello.txt

- Directy creation/deletion

- Large file write test

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

When the example runs successfully, the board shows the following menu, and a prompt will be display for select the corresponding test item.


.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   spi sdcard fatfs demo                     *
   *                                                             *
   *        1 - Create hello.txt                                 *
   *        2 - Read 1st line from hello.txt                     *
   *        3 - Directory related test                           *
   *        4 - Large file write test                            *
   *-------------------------------------------------------------*
   SD card has been mounted successfully
   1
   Create new file successfully, status=0
   Write file　operation is successfully
   2
   Open file successfully
   Hello, this is SPI SD card FATFS demo

   3
   Creating new directory succeeded
   Removing new directory succeeded
   4
   Create new file successfully, now writing.....
   Write file operation is successful
