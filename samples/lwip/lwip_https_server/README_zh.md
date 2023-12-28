# lwip_https_server

## 概述

本示例演示使用lwip和mbedtls实现https网页服务器的功能：

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口

## 工程配置

- 以太网端口配置：参考 [以太网通用工程设置](../doc/Ethernet_Common_Project_Settings_zh.md)
- 以太网DHCP配置
    - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:
      - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能
      - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能


## 运行示例

* 编译下载程序
* 串口终端显示如下信息：
```console
This is an ethernet demo: https server (Polling Usage)
LwIP Version: 2.1.2
Reference Clock: Internal Clock
Enet phy init passed !
Static IP: 192.168.100.10
Netmask  : 255.255.255.0
Gateway  : 192.168.100.1

************************************************
 mbedTLS HTTPS Server example
************************************************
Link Status: Down
Link Status: Down
Link Status: Up
Link Speed:  100Mbps
Link Duplex: Full duplex
```

使用浏览器客户端，输入https://192.168.100.10，回车后显示如下的页面：

 ![https_server](doc/lwip_mbedtls_https_server.png "https_server")
