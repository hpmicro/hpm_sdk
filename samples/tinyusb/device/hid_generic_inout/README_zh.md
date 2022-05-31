# USB HID 通用传输

## 概述

本示例工程展示USB HID通用传输回显

- PC 通过USB 发送USB HID Report，MCU接收后回发至PC

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用USB Type-C线缆连接PC USB端口和开发板USB0端口或者USB1端口

## 工程配置

---

- 文件[CMakeLists.txt](./CmakeLists.txt)中设置DBOARD_DEVICE_RHPORT_NUM为0或1，0表示USB0, 1表示USB1
- USB device 属性可通过tusb_config.h或CMakeListx.txt配置

## 运行现象

---

当工程正确运行后

* 进入相对路径:“samples\tinyusb\device\hid_generic_inout”
* 运行脚本: hid_echo.py
* 观察USB HID Write发log： PC 发送1024字节至MCU，首字节为output report id
  ![1646393298095.png](image/README_zh/1646393298095.png)
* 观察USB HID Read接log：PC 接收1024字节从MCU，首字节为input report id
  ![1646386827393.png](image/README_zh/1646386827393.png)
