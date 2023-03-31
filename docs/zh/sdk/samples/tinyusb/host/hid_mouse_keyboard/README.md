# USB HID HOST

## 概述

本示例工程展示USB HID HOST

## 硬件设置

- 使用USB Type-C线缆线连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C转Type-A线缆线连接开发板USB0端口和HID设备，如鼠标和键盘

## 运行现象

- 将程序下载至开发板运行，使用串口调试助手查看输出log

### 接键盘

- 开发板USB0端口接入键盘时，串口调试助手会显示如下数据：
```
TinyUSB Host HID Example
HID device address = 1, instance = 0 is mounted
HID Interface Protocol = Keyboard
HID device address = 1, instance = 1 is mounted
HID Interface Protocol = None
HID has 2 reports
```
- 当按压键盘时，将显示键盘发送的数据，例如输出hpmicro，回显如下：
```console
hpmicro
```

### 接鼠标

- 开发板USB0端口接入鼠标时，串口调试助手会显示如下数据：
```
TinyUSB Host HID Example
HID device address = 1, instance = 0 is mounted
HID Interface Protocol = Mouse
```
- 当移动或点击鼠标时，将显示鼠标发送的数据：
```console
(-25 -68 0)
(-18 -44 0)
(-12 -27 0)
(-7 -16 0)
(-1 -1 0)
(1 -2 0)
(5 -5 0)
```

