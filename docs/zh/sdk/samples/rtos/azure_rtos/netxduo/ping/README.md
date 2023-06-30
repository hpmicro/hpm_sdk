# Netx Duo Ping

## 概述

这个例程演示了netx的arp和icmp功能。它使能了对ping命令的回应。

## 硬件设置

无特殊设置

## 工程配置
- 可以通过将`CONFIG_NETX_ENABLE_DHCP`设置为0或者注释掉定义`CONFIG_NETX_ENABLE_DHCP`宏的语句来禁止DHCP功能。
- 禁用DHCP时，可以注释掉`set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)`语句，从而减少工程的源文件数量
- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"

## 运行现象

当工程正确运行后,链接状态会被周期性的打印到终端上,直到链接状态为Up,接收到ICMP包的数量也会被周期性的打印到终端上。以下消息会被打印到终端上:
```console
NetXDuo is running
IP address: 192.168.1.223
Mask: 255.255.255.0
received 0 ICMP packets
Enet phy init passes !
received 0 ICMP packets
Link Status: Down
received 0 ICMP packets
received 0 ICMP packets
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 1 ICMP packets
received 2 ICMP packets
received 3 ICMP packets
received 4 ICMP packets
```
