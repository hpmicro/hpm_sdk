# USBX host mouse

## Overview

The sample implements a usb host which can work with a usb mouse device.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on the development board USB0 to hid device with a USB Type-C convert Type-A cable

## Running the example

When the example runs successfully, move or press key of the mouse, following message is displayed in the terminal:
```console
USBX host HID Mouse example
USB device: vid=0x46d, pid=0xc542
Move to: x:20 y:-13
Move to: x:32 y:-21
Move to: x:29 y:-6
Move to: x:26 y:0
Move to: x:24 y:0
Move to: x:-29 y:-12
Move to: x:-41 y:-14
Move to: x:-34 y:-30
Move to: x:-24 y:-39
Move to: x:-11 y:-40
Move to: x:-10 y:-40
Left Click
Left Click
Right Click
Right Click
Middle Click
Middle Click
Wheel UP
Wheel Down
Wheel Down
Wheel Down
Wheel UP
Wheel UP
```
