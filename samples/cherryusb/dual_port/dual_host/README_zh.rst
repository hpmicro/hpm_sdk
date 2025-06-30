.. _host_dual_port:

Host Dual Port
============================

概述
------

本示例工程展示USB Host 2个usb口同时工作，支持HID（Keyboard和Mouse）和MSC设备（注意：U盘格式化的 `文件系统` 为FAT、FAT32或exFAT）。

  .. image:: ../../doc/UDisk_Format.png
     :alt: UDisk_Format

硬件设置
------------

- 使用USB Type-C线缆线连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C转Type-A线缆线连接开发板USB0端口

- 使用USB Type-C转Type-A线缆线连接开发板USB1端口

软件配置
------------

- 在CMakeLists.txt文件中，若未开启sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)，则FATFS支持Cacheable Buffer的数据传输；若开启sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)，则FATFS仅支持Noncacheable Buffer的数据传输。
- 当未开启sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)时，建议传输数据的Buffer地址CacheLine对齐，否则会影响传输性能。

运行现象
------------

- 将程序下载至开发板运行，使用串口调试助手查看输出log；

- 将U盘插入UBS0或USB1任意端口

    - 首先，会在U盘里面创建一个"cherryusb_msc_test.txt"文件，往文件里面写入字符串"cherryusb fatfs demo..."，总共100行，总长度为2500字节；然后将该文件的数据读取出来，总长度为2500。

    - 其次，会扫描U盘根目录下的目录和文件，将目录名和文件名打印出来，串口调试助手会显示如下数据：

    .. code-block:: console

        FATFS cherryusb mount succeeded!
        test fatfs write
        write success, write len：2500
        test fatfs read
        read success, read len：2500

        Direcotry Path: /
        Folder Name: LOST.DIR
        Folder Name: DCIM
        Folder Name: MIUI
        Folder Name: Android
        Folder Name: tencent
        File   Name: USBHost.txt
        File   Name: cherryusb_msc_test.txt


- 将键盘插入UBS0或USB1任意端口

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


- 将鼠标插入UBS0或USB1任意端口

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
