# Netx Duo Udp

## 概述

这个例程演示了netx的udp发送和接收功能。

## 硬件设置

无特殊设置

## 工程配置
- 可以通过将`CONFIG_NETX_ENABLE_DHCP`设置为0或者注释掉定义`CONFIG_NETX_ENABLE_DHCP`宏的语句来禁止DHCP功能。
- 禁用DHCP时，可以注释掉`set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)`语句，从而减少工程的源文件数量
- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"

## 运行现象

当工程正确运行后,程序会发送udp消息 **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "** 到**ECHO_SERVER_ADDRESS : ECHO_SERVER_PORT**(192.168.1.2:7777) 通过 **ECHO_SEND_PORT**(7777) 端口,然后会等待接收udp消息一段时间。 链接状态会被周期性的打印到终端上,直到链接状态为 **Up**。如果接收到了udp消息,那么这个消息也会被打印到终端上。以下消息会被打印到终端上:
```console
NetXDuo is running
IP address: 192.168.1.223
Mask: 255.255.255.0
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
No response from server 1
No response from server 2
No response from server 3
No response from server 4
Receive response: abcdefg 5
Receive response: abcdefg 6
Receive response: abcdefg 7
Receive response: abcdefg 8
Receive response: abcdefg 9
Receive response: abcdefg 10
Receive response: abcdefg 11
```
