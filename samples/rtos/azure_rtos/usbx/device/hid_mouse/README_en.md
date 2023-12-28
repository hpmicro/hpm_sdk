# USBX device hid mouse

## Overview

The example implements a hid mouse device. 

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

## Running the example

When the example runs successfully, the mouse will move in a rectangle, following message is displayed in the terminal:
```console
USBX device HID Mouse example

ux_device_stack_initialize
HID device activate

USB device is CONFIGURED
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
```
