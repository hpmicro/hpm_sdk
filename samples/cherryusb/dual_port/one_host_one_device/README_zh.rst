.. _one_host_one_device_dual_port:

One Host One Device Dual Port
==========================================================

概述
------

本示例工程展示USB Host 2个usb口同时工作，一个作为Host，一个作为Device。USB0作为Host支持HID（Keyboard和Mouse）设备，USB1作为U盘设备。

硬件设置
------------

- 使用USB Type-C线缆线连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C转Type-A线缆线连接开发板USB0端口

- 使用USB Type-C转Type-A线缆线连接开发板USB1端口

运行现象
------------

- 将程序下载至开发板运行，使用串口调试助手查看输出log

- 将键盘插入UBS0端口

    - 接入后，串口调试助手会显示如下数据：

    .. code-block:: console

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

        0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8


- 将鼠标插入UBS0端口

    - 接入后，串口调试助手会显示如下数据：

    .. code-block:: console

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

        0x01 0x00 0xfa 0xff 0x06 0x00 0x00 nbytes:7
        0x01 0x00 0xf8 0xff 0x07 0x00 0x00 nbytes:7
        0x01 0x00 0xf7 0xff 0x07 0x00 0x00 nbytes:7
        0x01 0x00 0xf5 0xff 0x09 0x00 0x00 nbytes:7
        0x01 0x00 0xf4 0xff 0x08 0x00 0x00 nbytes:7

- 将USB1口与PC相连接，PC将枚举出一个U盘设备。

    - 可以将文件copy至U盘，然后从U盘copy出来，可当做U盘使用。
