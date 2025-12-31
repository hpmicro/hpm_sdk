.. _cherryusb_otg:

USB OTG
================

Overview
--------

- This sample project demonstrates USB OTG functionality, where USB determines whether to act as Host or Device based on the ID signal.

- When USB0 port is floating or connected to a Device USB cable (male connector), the USB0 ID signal is high level, and USB0 acts as Device; when USB0 port is connected to a Host USB cable (female connector), the USB0 ID signal is low level, and USB0 acts as Host.

Running the example
---------------------

As Host
~~~~~~~

- Download the program to the development board and run it, use the serial port debugging assistant to view the output log

- Connect the Host USB cable (female connector) to the USB0 port, USB0 will switch to Host mode, and the serial port debugging assistant will print the following log:

.. code-block:: text

   CherryUSB OTG Sample...
   [I/usbotg_core] Switch to HOST mode
   [I/USB] EHCI HCIVERSION:0x0100
   [I/USB] EHCI HCSPARAMS:0x010011
   [I/USB] EHCI HCCPARAMS:0x0006
   [I/USB] EHCI ppc:1, n_ports:1, n_cc:0, n_pcc:0
   [I/USB] EHCI uses tt for ls/fs device

Connect Keyboard
>>>>>>>>>>>>>>>>

- When a keyboard is connected to the development board's USB0 port, the serial port debugging assistant will display the following data:

.. code-block:: text

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

- When pressing the keyboard, the data sent by the keyboard will be displayed:

.. code-block:: console

   Keyboard Data:0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8

Connect Mouse
>>>>>>>>>>>>>

- When a mouse is connected to the development board's USB0 port, the serial port debugging assistant will display the following data:

.. code-block:: text

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

- When moving or clicking the mouse, the data sent by the mouse will be displayed:

.. code-block:: console

   Mouse Data:0x01 0x00 0xfa 0xff 0x06 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf8 0xff 0x07 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf7 0xff 0x07 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf5 0xff 0x09 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf4 0xff 0x08 0x00 0x00 nbytes:7

As Device
~~~~~~~~~

- When USB0 port is floating or connected to a Device USB cable (male connector), USB0 will switch to Device mode, and the serial port debugging assistant will print the following log:

.. code-block:: text

   CherryUSB OTG Sample...
   [I/usbotg_core] Switch to DEVICE mode

- Connect the USB cable to the computer, the computer can automatically recognize and install the serial port driver, enumerating a device with one COM port

- Open the serial port debugging assistant software, select the enumerated COM port, and click to open the port

- Then input characters in the serial port debugging assistant, click send, and the sent characters will be echoed back in the serial port debugging assistant.