# Netx Duo Sntp客户端

## 概述

这个例程演示了netx的sntp客户端功能。它实现了一个sntp客户端。

## 硬件设置

无特殊设置

## 工程配置
- 可以通过将`CONFIG_NETX_ENABLE_DHCP`设置为0或者注释掉定义`CONFIG_NETX_ENABLE_DHCP`宏的语句来禁止DHCP功能。
- 禁用DHCP时，可以注释掉`set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)`语句，从而减少工程的源文件数量
- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"

## 运行现象

当工程正确运行后,链接状态会被周期性的打印到终端上,直到链接状态为Up。以下消息会被打印到终端上:
程序会以定点的方式去请求时间数据，当获取到时间后，会打印到终端上。注意：例程要求sntp服务器的root dispersion不能大于5秒。

```console
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
year [2023] :month [5]: day [5] :hour  [11]: min  [21]: second  [51]
SNTP updated
Time: 3892245711.435 sec.
```
