# USB MSC U 盘

## 概述

本示例工程展示U 盘访问操作

- MCU读取并列出U盘根目录文件及目录，并打印测试文件写入结果

## 硬件设置

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C转USB-A线缆连接U 盘和开发板USB0端口或者USB1端口

## 工程配置

- 文件[CMakeLists.txt](./CmakeLists.txt)中设置DBOARD_HOST_RHPORT_NUM为0或1，0表示USB0, 1表示USB1
- USB host 属性可通过tusb_config.h或CMakeListx.txt配置

## 运行现象

当工程正确运行后

* 观察U盘信息和文件写入结果

  ![1646400244250.png](image/README_zh/1646400244250.png)
