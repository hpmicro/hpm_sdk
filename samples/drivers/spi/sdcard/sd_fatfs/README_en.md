# SPI SDCard FATFS

## Overview

the sample use the SPI interface to operate SD card and use the FATFS file system, the sample needs to format the SD card as FAT32, The sample provides the following features:

- Create hello.txt
- Read 1st line from hello.txt
- Directy creation/deletion
- Large file write test

## Board Setting

- [SPI PINs](lab_board_app_spi_pin) Check the information according to the board model
- The SPI pins connect the SD card(module) pins

## Running the example

When the example runs successfully, the board shows the following menu, and a prompt will be display for select the corresponding test item.

```console
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



```