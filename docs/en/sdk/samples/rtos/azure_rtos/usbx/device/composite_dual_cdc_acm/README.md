# USBX composite_dual_cdc_acm

## Overview

The usbx composite_dual_cdc_acm example enable a usb device which has two cdc acm interfaces. When it is configured, we can find two virtual COM ports on PC. We can use COM assister program to operate them.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

## Running the example

Plug usb0 to PC, we can find two new COM port on computer device manager. Start up two COM assisters, open each COM port, and send data to them. One will send "123456\n" back to pc each time computer send data, well the other one will send "abcdef\n" instead.
When the example runs successfully, following message is displayed in the terminal:
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
Start USBX device composite example...
CDC ACM1 device activate
CDC ACM2 device activate
```

When PC send some data to device interface0, pc will receive a string "123456\r\n" after the string just sent to the device.

When PC send some data to device interface1, pc will receive a string "abcdef\r\n" after the string just sent to the device.

At the same time, device console will output "SEND: " + data send to host:
At the same time, device console will output:
```console
SEND: 123abcdef

SEND: 345abcdef

```