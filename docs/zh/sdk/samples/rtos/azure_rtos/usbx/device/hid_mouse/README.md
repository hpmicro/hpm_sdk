# USBX 鼠标设备

## 概述

该工程实现了一个鼠标设备。

## 硬件设置

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

## 运行现象

当工程正确运行后，鼠标会以矩形的轨迹进行运动。串口终端会输出如下信息：
```console
USBX device HID Mouse example

ux_device_stack_initialize
HID device activate

USB device is CONFIGURED
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
ux_device_class_hid_event_set success
```
