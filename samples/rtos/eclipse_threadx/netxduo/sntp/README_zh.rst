.. _netx_duo_sntp_client:

Netx Duo Sntp客户端
================================

概述
------

这个例程演示了netx的sntp客户端功能。它实现了一个sntp客户端。客户端会向服务器192.168.1.2获取时间信息。

硬件设置
------------

用网线连接开发板和电脑/路由器/交换机

工程配置
------------

- 以太网端口配置：参考 `以太网通用工程设置 <../../../../lwip/doc/Ethernet_Common_Project_Settings_zh.rst>`_

- 可以通过`set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)`的语句来使能DHCP功能。

运行现象
------------

当工程正确运行后,链接状态会被周期性的打印到终端上,直到链接状态为Up。以下消息会被打印到终端上:
程序会以定点的方式去请求时间数据，当获取到时间后，会打印到终端上。注意：例程要求sntp服务器的root dispersion不能大于5秒。


.. code-block:: console

   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   year [2023] :month [5]: day [5] :hour  [11]: min  [21]: second  [51]
   SNTP updated
   Time: 3892245711.435 sec.

