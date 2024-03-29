# USBX 主机例程-U盘

## 概述

该工程实现了一个可以使用U盘的USB主机。当u盘插入后，程序会扫描文件系统，并将信息循环打印到终端上。

## 硬件设置

- 使用USB Type-C线缆线连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C转Type-A线缆线连接开发板USB0端口和U盘。

## 注意事项

- U盘的文件系统格式必须是FAT16/32或者exFAT。
- 程序只读取了运行后第一个插入的设备。

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```console
USBX host mass storage example
USB device: vid=0x781, pid=0x5591
find the udisk
Find Dir: System Volume Information
Find File: 1.txt
find the udisk
Find Dir: System Volume Information
Find File: 1.txt
find the udisk
Find Dir: System Volume Information
Find File: 1.txt
```
