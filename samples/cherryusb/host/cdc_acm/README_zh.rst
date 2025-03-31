.. _cdc_acm_host:

CDC ACM Host
========================

概述
------

本示例工程展示USB CDC ACM HOST示例。

硬件设置
------------

- 需要两块开发板，一块板子作为CDC ACM Host，另一块板子作为CDC ACM Device。

- CDC ACM Device的sample的路径为：samples/cherryusb/device/cdc_acm/cdc_acm_vcom。Host即为本示例。

- 将Host和Device的USB0端口相连接。

运行现象
------------

- 将程序下载至开发板运行，使用串口调试助手查看输出log。

- 按压Host开发板的PBUT或USER_KEYA按键，HOST将会发出“HPMicro”字符串给Device，Device收到后，将返回“HPMicro”字符串给Host。


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

