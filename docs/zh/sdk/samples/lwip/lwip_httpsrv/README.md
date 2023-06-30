# http服务器

## 概述

本示例演示使用lwip http实现网页服务器的功能

注：

* 重复刷新网页，可能会出现异常

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口

## 工程配置

- 在文件 `CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"

## 运行示例

* 编译下载程序
* 串口终端显示如下信息：

```console
This is an ethernet demo: HTTP Server (Polling Usage)
LwIP Version: 2.1.2
Reference Clock: Internal Clock
Enet phy init passes !
Static IP: 10.10.10.10
NETMASK  : 255.255.255.0
Gateway  : 10.10.10.1
Link Status: Down
Link Status: Down
Link Status: Up
Link Speed:  100Mbps
Link Duplex: Full duplex
```

使用浏览器客户端，输入https://10.10.10.10，回车后显示如下的页面：

注：此示例可以成功运行，通过使用360或IE内核的浏览器

 ![https_server](../../../../../assets/sdk/samples/lwip_httpsrv.png)
