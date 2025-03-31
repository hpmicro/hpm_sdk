.. _cdc_acm_host:

CDC ACM Host
========================

Overview
--------

This example project shows USB CDC ACM HOST.

Board Setting
-------------

- We need two development boards, one board as the CDC ACM Host and the other board as the CDC ACM Device.

- CDC ACM Device sample path is `samples/cherryusb/device/cdc_acm/cdc_acm_vcom`, and host is this example.

- Connect the USB0 ports of the Host and Device.

Running the example
-------------------

- Download the program to the development board and run it. Use the serial debugging assistant to view the output log

- Press the PBUT or USER_KEYA button on the Host development board, and the HOST will send the "HPMicro" string to the Device. Once the Device receives it, it will return the "HPMicro" string to the Host.


.. code-block:: console

   Start usb host task...
   [I/USB] EHCI HCIVERSION:0x0100
   [I/USB] EHCI HCSPARAMS:0x010011
   [I/USB] EHCI HCCPARAMS:0x0006
   [I/USB] EHCI ppc:1, n_ports:1, n_cc:0, n_pcc:0
   [I/USB] EHCI uses tt for ls/fs device
   [I/usbh_hub] New high-speed device on Bus 0, Hub 1, Port 1 connected
   [I/usbh_core] New device found,idVendor:34b7,idProduct:ffff,bcdDevice:0100
   [I/usbh_core] The device has 1 bNumConfigurations
   [I/usbh_core] The device has 2 interfaces
   [I/usbh_core] Enumeration success, start loading class driver
   [I/usbh_core] Loading cdc_acm class driver
   [I/usbh_cdc_acm] Ep=01 Attr=02 Mps=512 Interval=00 Mult=00
   [I/usbh_cdc_acm] Ep=81 Attr=02 Mps=512 Interval=00 Mult=00
   [I/usbh_cdc_acm] Register CDC ACM Class:/dev/ttyACM0
   [I/usbh_core] Loading cdc_data class driver
   send length:8
   receive length:8
   receive data: HPMicro

   send length:8
   receive length:8
   receive data: HPMicro

   send length:8
   receive length:8
   receive data: HPMicro

   send length:8
   receive length:8
   receive data: HPMicro

