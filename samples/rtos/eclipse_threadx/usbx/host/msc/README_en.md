# USBX host msc

## Overview

The sample implements a usb host which can work with a usb msc device. Once device inserted, the program will scan the file system and print info to the console in a loop.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on the development board USB0 to usb disk with a USB Type-C convert Type-A cable

## Notes
- Make sure the msc device is formatted as FAT16/32 or EXFAT.
- The sample only scan the first inserted device.

## Running the example

When the example runs successfully, following message is displayed in the terminal:
```console
USBX host mass storage example
USB device: vid=0x781, pid=0x5591
find the udisk
Find Dir: System Volume Information
Find File: 1.txt
find the udisk
Find Dir: System Volume Information
Find File: 1.txt
find the udisk
Find Dir: System Volume Information
Find File: 1.txt
```
