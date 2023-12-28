# USB HID HOST

## Overview

This  example project shows USB HID HOST

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on the development board USB0 to hid device with a USB Type-C convert Type-A cable

## Running the example

- Download the program to the development board to run, and use the serial port debugging assistant to view the output log

### Connected with a keyboard
- When the USB port0 of the development board is connected with a keyboard, the serial port debugging assistant will display the following data:
```
TinyUSB Host HID Example
HID device address = 1, instance = 0 is mounted
HID Interface Protocol = Keyboard
HID device address = 1, instance = 1 is mounted
HID Interface Protocol = None
HID has 2 reports
```
- When the keyboard is pressed, the data sent by the keyboard will be displayed, for example, input "hpmicro":
```console
hpmicro
```

### Connected with a mouse
- When the USB port0 of the development board is connected with a mouse, the serial port debugging assistant will display the following data:
```
TinyUSB Host HID Example
HID device address = 1, instance = 0 is mounted
HID Interface Protocol = Mouse
```
- When moving or clicking the mouse, the data sent by the mouse will be displayed:
```console
(-25 -68 0)
(-18 -44 0)
(-12 -27 0)
(-7 -16 0)
(-1 -1 0)
(1 -2 0)
(5 -5 0)
```


