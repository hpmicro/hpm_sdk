.. _cherryusb_otg:

USB OTG
================

概述
------

- 本示例工程展示USB OTG功能，USB根据ID信号来决定作为Host还是Device。

- 当USB0口悬空或接入Device的USB电缆线（公头），USB0 ID信号为高电平，USB0作为Device；当USB0口接入Host的USB电缆线（母头），USB0 ID信号为低电平，USB0作为Host。

运行现象
------------

作为Host
~~~~~~~~~~~~~~~~~

- 将程序下载至开发板运行，使用串口调试助手查看输出log

- USB0口接入Host的USB电缆线（母头），USB0会切换成Host，串口调试助手打印log如下：


.. code-block:: text

   CherryUSB OTG Sample...
   [I/usbotg_core] Switch to HOST mode
   [I/USB] EHCI HCIVERSION:0x0100
   [I/USB] EHCI HCSPARAMS:0x010011
   [I/USB] EHCI HCCPARAMS:0x0006
   [I/USB] EHCI ppc:1, n_ports:1, n_cc:0, n_pcc:0
   [I/USB] EHCI uses tt for ls/fs device


接键盘
>>>>>>>>>

- 开发板USB0端口接入键盘时，串口调试助手会显示如下数据：


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

- 当按压键盘时，将显示键盘发送的数据：


.. code-block:: console

   Keyboard Data:0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
   Keyboard Data:0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8


接鼠标
>>>>>>>>>

- 开发板USB0端口接入鼠标时，串口调试助手会显示如下数据：


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

- 当移动或点击鼠标时，将显示鼠标发送的数据：


.. code-block:: console

   Mouse Data:0x01 0x00 0xfa 0xff 0x06 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf8 0xff 0x07 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf7 0xff 0x07 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf5 0xff 0x09 0x00 0x00 nbytes:7
   Mouse Data:0x01 0x00 0xf4 0xff 0x08 0x00 0x00 nbytes:7


作为Device
~~~~~~~~~~~~~~~~~

- USB0口悬空或接入Device的USB电缆线（公头），USB0会切换成Device，串口调试助手打印log如下：


.. code-block:: text

   CherryUSB OTG Sample...
   [I/usbotg_core] Switch to DEVICE mode

- 将USB电缆线接入电脑，电脑可自动识别并安装串口驱动，枚举出具有一个com口的设备

- 打开串口调试助手软件，选择枚举出来的com口，点击打开串口

- 然后在串口调试助手中输入字符，点击发送，串口调试助手中会回显所发送的字符。
