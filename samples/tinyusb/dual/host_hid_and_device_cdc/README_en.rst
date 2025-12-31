.. _tinyusb_dual_port:

TINYUSB DUAL PORT
========================

Overview
--------

- This sample project demonstrates dual USB ports, where USB0 acts as a Device functioning as a CDC-ACM Device, and USB1 acts as a Host functioning as an HID Host.

- The USB host receive reports from HID device and print it out over USB Device CDC interface.

Hardware Setup
--------------

- USB0 port acts as Device, connected to PC.

- USB1 port acts as HID Host, connected to keyboard.

Running Phenomenon
------------------

- After downloading and running the program on the development board, the PC can enumerate a CDC-ACM device.

- Using a serial debugging assistant to open the serial port, typing any characters will result in the serial debugging assistant echoing the input characters.

- When a keyboard is connected to the USB1 port, pressing keyboard keys will display the keyboard characters in the serial debugging assistant.