# USB CDC MSC

## Overview

This  example project shows a USB CDC virtual serial port and a MSC virtual U disk

- a virtual serial port is listed on Device Manager
- a virtual u disk is shown in Resouce Manager

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

## Running the example

When the project runs correctly

- PC automatically installs a USB-to-serial port driver and enumerate a composite device with a virtual serial port and a virtual U disk
- Enter any letters on the serial terminal, and the USB device will echo the letters back to PC
- Open Reource Manager to find a virtual disk named "TinyUSB MSC" and access it temporarily before the MCU system powers off or restarts
