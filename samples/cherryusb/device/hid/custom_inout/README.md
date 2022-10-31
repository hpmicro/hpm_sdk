# USB HID Custom Inout

## Overview

This example project shows USB hid custom transfers

- PC transmits a HID report to MCU,  and then MC send it back to PC

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

## Running the example

- Download the program and run. The computer can automatically recognize and enumerate a vendor-defined device.
- Run
```
python hid_echo.py
```
- View  "USB HID Write" log： PC transmits 64 bytes to MCU through USB and the first byte is the output report id
- View "USB HID Read" log: PC receives 64 bytes to MCU through USB and the first byte is the output report id
