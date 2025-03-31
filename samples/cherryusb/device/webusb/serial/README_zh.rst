.. _webusb_serial:

WebUSB Serial
====================

概述
------

本示例工程展示 WebUSB Serial 设备。

本示例中，USB设备有3个interface，其中的2个用于CDC Class Device，其中的1个用于Vendor Class。它们共同构成了WebUSB Serial设备。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装驱动，设备管理器可查看得到 1个 ``USB 串行设备(COMxx)`` 和 1个 ``HPMicro WebUSB`` 。

    .. image:: ./doc/PC_device_manager.png
       :alt: PC_device_manager.png

- 请打开网页（使用TinyUSB的一个服务）： https://example.tinyusb.org/webusb-serial/index.html 。

- 点击网页 `Connect` 按钮，然后选择 `HPMicro WEBUSB DEMO` 设备，然后点击 `连接` 按钮。网页将与设备建立连接，开发板LED将点亮。

    .. image:: ./doc/Connect.png
       :alt: Connect.png

- 用户在 `Sender` 窗口中输入字符， `Receiver` 窗口将会回显同样的字符。

- 点击网页 `Disconnect` 按钮，网页将与设备断开连接，开发板LED将熄灭。

- 用户打开串口调试助手，选择被枚举出来的COM口，发送字符，可以看到串口调试助手回显同样的字符。

    .. image:: ./doc/UartAssist.png
       :alt: UartAssist.png