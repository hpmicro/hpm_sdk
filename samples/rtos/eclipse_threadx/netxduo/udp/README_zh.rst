.. _netx_duo_udp:

Netx Duo Udp
========================

概述
------

这个例程演示了netx的udp发送和接收功能。

硬件设置
------------

用网线连接开发板和电脑/路由器/交换机

工程配置
------------

- 以太网端口配置：参考 `以太网通用工程设置 <../../../../lwip/doc/Ethernet_Common_Project_Settings_zh.rst>`_

- 可以通过`set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)`的语句来使能DHCP功能。

- 当使能DHCP功能时，需要注意宏ECHO_SERVER_ADDRESS代表的ip地址可以通过网关进行访问

运行现象
------------

当工程正确运行后,程序会发送udp消息 **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "**  到 **ECHO_SERVER_ADDRESS : ECHO_SERVER_PORT** (192.168.1.2:7777) 通过 **ECHO_SEND_PORT** (7777) 端口,然后会等待接收udp消息一段时间。 链接状态会被周期性的打印到终端上,直到链接状态为 **Up** 。如果接收到了udp消息,那么这个消息也会被打印到终端上。以下消息会被打印到终端上:

.. code-block:: console

   NetXDuo is running
   IP address: 192.168.1.223
   Mask: 255.255.255.0
   Enet phy init passed !
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

