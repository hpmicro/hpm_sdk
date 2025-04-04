.. _usb_cdc_msc:

USB CDC MSC
======================

概述
------

本示例工程展示USB CDC设备虚拟串口以及MSC 虚拟U盘

- 通过PC设备管理器查看得到一个串口

- 通过PC资源管理器查看得到一个U盘

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

运行现象
------------

当工程正确运行后

- 电脑可自动识别并安装串口驱动并枚举出一个具有一个com以及一个U盘的复合设备

- 查找串口号，并在串口终端输入任意字符，同时MCU USB device会回显来自PC的数据

- 打开资源管理器，找到名为"TinyUSB MSC"的U盘, 可进行访问并且临时保存文件在系统断电或重启以前
