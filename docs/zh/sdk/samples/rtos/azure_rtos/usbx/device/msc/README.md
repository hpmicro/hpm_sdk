# USBX msc设备

## 概述

本例程实现了一个msc设备。

## 硬件设置

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

## 运行现象
主机上应当识别到一个磁盘，它的容量应当是26KB，可以拷贝文件到磁盘，或者从磁盘拷贝文件到本地硬盘中。
当工程正确运行后，串口终端会输出如下信息：

```console
USBX device mass storage example
USB MSD device activate
```
