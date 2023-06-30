# Netx Duo Tcp客户端

## 概述

这个例程演示了netx的tcp客户端功能。

## 硬件设置

无特殊设置

## 工程配置
- 可以通过将`CONFIG_NETX_ENABLE_DHCP`设置为0或者注释掉定义`CONFIG_NETX_ENABLE_DHCP`宏的语句来禁止DHCP功能。
- 禁用DHCP时，可以注释掉`set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)`语句，从而减少工程的源文件数量
- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"
- 使能DHCP时，请确保服务端地址`SAMPLE_IPV4_ADDRESS`是可访问的
## 运行现象

当工程正确运行后,程序会尝试链接到**SAMPLE_IPV4_ADDRESS : ECHO_SERVER_PORT**(192.168.1.2:7777)。一旦连接到了服务端，程序会发送消息 **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "** 到服务端,然后会等待服务端返回数据直到真正收到数据或者服务端关闭了链接。 物理链接状态会被周期性的打印到终端上,直到链接状态为 **Up**。如果接收到了消息,那么这个消息也会被打印到终端上;如果服务端主动关闭了链接,那么tcp接收函数会立即返回。以下消息会被打印到终端上:
```console
NetXDuo is running
IPv4 address: 192.168.1.223
Mask: 255.255.255.0
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
Connecting to server: Connected
Receive response: abcdefg
Receive response: abcdefg
Receive response: abcdefg
Receive response: abcdefg
No response from server
```
