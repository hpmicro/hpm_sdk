.. _lvgl_indev_usb_keyboard_mouse:

lvgl_indev_usb_keyboard_mouse
====================================

Overview
--------

This example demonstrates the implementation of mouse and keyboard input functionality using LVGL graphics library with USB HOST HID protocol on embedded systems. Key features include:

- Support for USB HID mouse coordinate movement and click events
- Support for USB HID keyboard input
- Supports LVGL v9 and v8 display and input device frameworks with automatic version adaptation
- Integration with FreeRTOS real-time operating system
- Interactive text box demonstration program

Notes
------

- USB stack initialization configures keyboard and mouse devices to operate in boot protocol

- In boot protocol mode:

  - For keyboard devices: Only standard keyboard scan code set is supported, modifier keys (e.g. Ctrl, Alt) are not supported
  - For mouse devices: Only basic pointer movement and button click functionality is supported

Hardware Requirements
---------------------

- RGB/MIPI interface display
- USB mouse/keyboard peripherals

Hardware Configuration
----------------------

- Connect the supported display to the development board (RGB display by default). To use other display types, modify `board/xxx/CMakeLists.txt` accordingly
- Connect input devices to USB0 port

Build Types
-----------

- flash_sdram_xip

- flash_sdram_xip_release

Operation
---------

- When the program runs successfully:
  - A mouse cursor and editable text box will be displayed on the screen
- Mouse movement: Move the cursor by operating the USB mouse
- Keyboard input: Type on the keyboard to display text in the input box
- When mouse/keyboard devices are connected, the console will output the following information:

.. code-block:: console

    [I/USB] EHCI HCIVERSION:0x0100
    [I/USB] EHCI HCSPARAMS:0x010011
    [I/USB] EHCI HCCPARAMS:0x0006
    [I/USB] EHCI ppc:1, n_ports:1, n_cc:0, n_pcc:0
    [I/USB] EHCI uses tt for ls/fs device
    [I/usbh_hub] New full-speed device on Bus 0, Hub 1, Port 1 connected
    [I/usbh_core] New device found,idVendor:1ea7,idProduct:0066,bcdDevice:0200
    [I/usbh_core] The device has 1 bNumConfigurations
    [I/usbh_core] The device has 2 interfaces
    [I/usbh_core] Enumeration success, start loading class driver
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=81 Attr=03 Mps=8 Interval=04 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input0
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=82 Attr=03 Mps=8 Interval=02 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input1
    [I/usbh_hid] Unregister HID Class:/dev/input0
    [I/usbh_hid] Unregister HID Class:/dev/input1
    [I/usbh_hub] Device on Bus 0, Hub 1, Port 1 disconnected
    [I/usbh_hub] New full-speed device on Bus 0, Hub 1, Port 1 connected
    [I/usbh_core] New device found,idVendor:1ea7,idProduct:0066,bcdDevice:0200
    [I/usbh_core] The device has 1 bNumConfigurations
    [I/usbh_core] The device has 2 interfaces
    [I/usbh_core] Enumeration success, start loading class driver
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=81 Attr=03 Mps=8 Interval=04 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input0
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=82 Attr=03 Mps=8 Interval=02 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input1
