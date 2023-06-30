# Netx Duo Ftp服务器

## 概述

这个例程演示了netx的ftp服务器功能。它实现了一个ftp服务器。

## 硬件设置

无特殊设置

## 工程配置
- 可以通过将`CONFIG_NETX_ENABLE_DHCP`设置为0或者注释掉定义`CONFIG_NETX_ENABLE_DHCP`宏的语句来禁止DHCP功能。
- 禁用DHCP时，可以注释掉`set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)`语句，从而减少工程的源文件数量
- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"

## 运行现象

当工程正确运行后,链接状态会被周期性的打印到终端上,直到链接状态为Up。以下消息会被打印到终端上:
当访问ftp服务器时,默认的用户名是“name”,默认的密码是“password”,确认后ftp客户端就可以创建、上传、下载、删除服务器端的文件了。需要注意的是，ftp服务器的容量是有限的，默认是128kB。

```console
FTP Server create success, available storage area is 1310720 bytes
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
Server started!
Logged in!
```
