# USB HID 自定义传输

## 概述

本示例工程展示USB HID自定义传输回显

- PC 通过USB 发送USB HID Report，MCU接收后回发至PC

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

## 运行现象

- 将程序下载至开发板运行，电脑可自动识别并枚举出一个厂商自定义的设备
- 运行脚本:
```
python hid_echo.py
```
- 观察USB HID Write发log： PC 发送64字节至MCU，首字节为output report id
- 观察USB HID Read接log：PC 接收64字节从MCU，首字节为input report id