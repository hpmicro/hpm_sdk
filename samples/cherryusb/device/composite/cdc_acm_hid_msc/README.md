# USB COMPOSITE DEVICE

## Overview

This example project shows USB composite devcie.

- PC sees a USB disk via Device Manager
- PC sees a serial port via Device Manager
- PC sees a mouse via Device Manager

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

## Running the example

- Download the program and run. The computer can automatically recognize and enumerate a USB disk device, a mouse device and a com port.
- Double click to open the USB disk. Format the USB disk according to the computer prompts to get the USB disk of about 500KB in size. You can copy a file to the USB disk, and then copy it from the USB disk.
- Pressing the PBUTN button on the board is equivalent to moving the mouse to the right.
- Open the "Serial Port Debugging Assistant" software, check DTR, select the enumerated com port, and click to open the serial port. Then enter characters in the "Serial Port Debugging Assistant", click Send, and the characters sent will be echoed in the "Serial Port Debugging Assistant".
