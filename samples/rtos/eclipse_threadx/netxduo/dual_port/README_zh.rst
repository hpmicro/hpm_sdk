.. _netx_duo_dual_port:

Netx Duo 双网卡
==========================

概述
------

这个例程演示了netx的双网卡功能。它使能了对ping命令的回应，并且在网卡0上开启了tcp client,在网卡1上开启了tcp server。

硬件设置
------------

用网线连接开发板和电脑/路由器/交换机

工程配置
------------

- 以太网端口配置：参考 `以太网通用工程设置 <../../../../lwip/doc/Ethernet_Common_Project_Settings_zh.rst>`_

- 此工程只支持静态IP地址配置。如果有dhcp需求，请参考其他netxduo例程添加dhcp相关代码。

运行现象
------------

当工程正确运行后,链接状态会被周期性的打印到终端上,直到链接状态为Up,接收到ICMP包的数量也会被周期性的打印到终端上。
对于网卡0，程序会尝试链接到 **SERVER_IPV4_ADDRESS : ECHO_SERVER_PORT** (192.168.100.3:7777)。一旦连接到了服务端，程序会发送消息 **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "**  到服务端,然后会等待服务端返回数据直到真正收到数据或者服务端关闭了链接。:
对于网卡1，程序会监听端口 **ECHO_SERVER_PORT** (7777)。一旦有一个客户端连接到了服务端，程序会等待客户端发送数据直到收到数据、10秒没收到数据或者客户端关闭了链接。如果收到了客户端的数据，程序会将同样的数据返回给客户端，随后关闭链接。
以下消息会被打印到终端上。

.. code-block:: console

Enet1 Reference Clock: Internal Clock
NetXDuo is running
ETH0 IP address: 192.168.100.223
ETH0 Mask: 255.255.255.0
ETH1 IP address: 192.168.99.223
ETH1 Mask: 255.255.255.0
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
Enet RGMII phy init passed !
Enet RMII phy init passed !
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
ETH0 Link Status: Down
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
ETH0 Link Status: Up
ETH0 Link Speed:  1000Mbps
ETH0 Link Duplex: Full duplex
ETH0 received 41 ICMP packets, ETH1 received 0 ICMP packets
ETH0 received 61 ICMP packets, ETH1 received 0 ICMP packets
ETH0 received 81 ICMP packets, ETH1 received 0 ICMP packets

