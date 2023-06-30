# USB RNDIS DEVICE - HTTP SERVER

## 概述

本示例工程展示USB RNDIS设备实现HTTP SERVER功能

- PC 通过设备管理器查看得到一个枚举的网口，MAC地址为20:89:84:6A:96:AA。
- 应用程序使用LWIP协议栈虚拟出了一个网口，MAC地址为20:89:84:6A:96:00，IP地址为192.168.7.1。实现了DHCP、DNS和HTTP服务器，PC可通过枚举出来的网口进行网页访问。

## 硬件设置

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

## 运行现象

- 将程序下载至开发板运行，电脑将枚举出一个网口设备，属性如下：
![ethernet_property.png](../../../../../../../assets/sdk/samples/cherryusb/ethernet_property.png)

- 断开其他网络连接，仅保留枚举出来的网口正常工作，打开Microsoft EDGE浏览器，输入网址：http://192.168.7.1 或 http://rndis.hpm，将打开网页如下：
![website.png](../../../../../../../assets/sdk/samples/cherryusb/website.png)
