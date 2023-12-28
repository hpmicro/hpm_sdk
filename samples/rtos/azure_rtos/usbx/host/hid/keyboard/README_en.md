# USBX host keyboard

## Overview

The sample implements a usb host which can work with a usb keyboard device.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on the development board USB0 to hid device with a USB Type-C convert Type-A cable

## Running the example

When the example runs successfully, plug in the keyboard, when press a key, following message is displayed in the terminal:
```console
----------------------------------------------------------------------
$$\   $$\ $$$$$$$\  $$\      $$\ $$\
$$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
$$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
$$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
$$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
$$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
$$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
\__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
----------------------------------------------------------------------
USBX host HID Keyboard example
USB device: vid=0x5ac, pid=0x24f
USB device: vid=0x5ac, pid=0x24f
The keyboard is alive
```
When press key, following message is displayed in the terminal:
```console
Input: a
```
