# USBX 主机例程-鼠标

## 概述

该工程实现了一个可以使用鼠标的USB主机。

## 硬件设置

- 使用USB Type-C线缆线连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C转Type-A线缆线连接开发板USB0端口和鼠标。

## 运行现象

当工程正确运行后，移动鼠标或者按下数据按键或滑动滚轮，串口终端会输出如下信息：
```console
USBX host HID Mouse example
USB device: vid=0x46d, pid=0xc542
Move to: x:20 y:-13
Move to: x:32 y:-21
Move to: x:29 y:-6
Move to: x:26 y:0
Move to: x:24 y:0
Move to: x:-29 y:-12
Move to: x:-41 y:-14
Move to: x:-34 y:-30
Move to: x:-24 y:-39
Move to: x:-11 y:-40
Move to: x:-10 y:-40
Left Click
Left Click
Right Click
Right Click
Middle Click
Middle Click
Wheel UP
Wheel Down
Wheel Down
Wheel Down
Wheel UP
Wheel UP
```
