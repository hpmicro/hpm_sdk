.. _netx_duo_ping:

Netx Duo Ping
==========================

概述
------

这个例程演示了netx的arp和icmp功能。它使能了对ping命令的回应。

硬件设置
------------

用网线连接开发板和电脑/路由器/交换机

工程配置
------------

- 以太网端口配置：参考 `以太网通用工程设置 <../../../../lwip/doc/Ethernet_Common_Project_Settings_zh.rst>`_

- 可以通过`set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)`的语句来使能DHCP功能。

运行现象
------------

当工程正确运行后,链接状态会被周期性的打印到终端上,直到链接状态为Up,接收到ICMP包的数量也会被周期性的打印到终端上。以下消息会被打印到终端上:

.. code-block:: console

   NetXDuo is running
   IP address: 192.168.1.223
   Mask: 255.255.255.0
   received 0 ICMP packets
   Enet phy init passed !
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

