.. _hid_mouse_remote_wakeup:

HID Mouse Remote Wakeup
==============================================

概述
------

本示例工程展示USB HID鼠标的Remote Wakeup功能。

- PC 通过设备管理器查看得到一个鼠标

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并枚举出一个鼠标设备。

- 按压板上的PBUTN/KEYA按钮，等效于鼠标向右移动。

- 让电脑进入休眠状态后，再次按压上的PBUTN/KEYA按钮，电脑将被唤醒。
