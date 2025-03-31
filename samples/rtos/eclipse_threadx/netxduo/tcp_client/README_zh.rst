.. _netx_duo_tcp_client:

Netx Duo Tcp客户端
==============================

概述
------

这个例程演示了netx的tcp客户端功能。

硬件设置
------------

用网线连接开发板和电脑/路由器/交换机,并确保`SERVER_IPV4_ADDRESS`是可访问的。

工程配置
------------

- 以太网端口配置：参考 `以太网通用工程设置 <../../../../lwip/doc/Ethernet_Common_Project_Settings_zh.rst>`_

- 可以通过`set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)`的语句来使能DHCP功能。

运行现象
------------

当工程正确运行后,程序会尝试链接到 **SERVER_IPV4_ADDRESS : ECHO_SERVER_PORT** (192.168.1.2:7777)。一旦连接到了服务端，程序会发送消息 **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "**  到服务端,然后会等待服务端返回数据直到真正收到数据或者服务端关闭了链接。 物理链接状态会被周期性的打印到终端上,直到链接状态为 **Up** 。如果接收到了消息,那么这个消息也会被打印到终端上;如果服务端主动关闭了链接,那么tcp接收函数会立即返回。以下消息会被打印到终端上:

.. code-block:: console

   NetXDuo is running
   IPv4 address: 192.168.1.223
   Mask: 255.255.255.0
   Enet phy init passed !
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

