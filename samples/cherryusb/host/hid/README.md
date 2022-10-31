# USB HID HOST

## Overview

This  example project shows USB HID HOST

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

## Running the example

- Download the program to the development board to run, and use the serial port debugging assistant to view the output log

### Connected with a keyboard
- When the USB port0 of the development board is connected with a keyboard, the serial port debugging assistant will display the following data:
```
[D/USB] Port change:0x02
[D/USB] Port 1 change
[D/USB] port 1, status:0x101, change:0x01
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[I/USB] New low-speed device on Hub 1, Port 1 connected
[I/USB] New device found,idVendor:413c,idProduct:2113,bcdDevice:0108
[I/USB] The device has 2 interfaces
[I/USB] Enumeration success, start loading class driver
[I/USB] Loading hid class driver
[I/USB] Ep=81 Attr=03 Mps=8 Interval=24 Mult=00
[I/USB] Register HID Class:/dev/input0
[E/USB] do not support Class:0x03,Subclass:0x00,Protocl:0x00
mount a keyboard
```
- When the keyboard is pressed, the data sent by the keyboard will be displayed:
```
0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
```

### Connected with a mouse
- When the USB port0 of the development board is connected with a mouse, the serial port debugging assistant will display the following data:
```
[D/USB] Port change:0x02
[D/USB] Port 1 change
[D/USB] port 1, status:0x100, change:0x01
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[D/USB] Port 1, status:0x101, change:0x00
[I/USB] New full-speed device on Hub 1, Port 1 connected
[I/USB] New device found,idVendor:046d,idProduct:c542,bcdDevice:0302
[I/USB] The device has 1 interfaces
[I/USB] Enumeration success, start loading class driver
[I/USB] Loading hid class driver
[I/USB] Ep=82 Attr=03 Mps=8 Interval=04 Mult=00
[I/USB] Register HID Class:/dev/input0
mount a mouse
```
- When moving or clicking the mouse, the data sent by the mouse will be displayed:
```
0x01 0x00 0xfa 0xff 0x06 0x00 0x00 nbytes:7
0x01 0x00 0xf8 0xff 0x07 0x00 0x00 nbytes:7
0x01 0x00 0xf7 0xff 0x07 0x00 0x00 nbytes:7
0x01 0x00 0xf5 0xff 0x09 0x00 0x00 nbytes:7
0x01 0x00 0xf4 0xff 0x08 0x00 0x00 nbytes:7
```


