.. _segger_sysview:

Segger SYSVIEW
====================

概述
------

Segger System View 示例程序演示SysView的基本API使用过程，其中包括如下系统类型:

- baremetal

- freertos

- ucos_iii

硬件设置
------------

- 使用J-Link（若有板载调试器，需要将其断开，TCK/TMS/TRST）

注意事项
------------

- 使用 SystemView 软件进行调试时，无论采用何种调试环境（如 SES、IAR、Keil 等），都必须使用 **J-Link 原生驱动** 连接目标设备。**不可使用 J-Link 的 OpenOCD 模式**，否则 SystemView 将无法正常连接并采集数据。

运行现象
------------

- 将开发板上电

- 下载编译好的示例程序

- 通过SystemView V3.62及以上版本连接开发板

- 填写正确的RTT control block地址，该地址可以在map文件中找到（符号: \_SEGGER\_RTT）

- SystemView软件中打开Start Recording

- 复位开发板

- 正确运行时

SystemView软件中可以看到实时的系统运行情况

