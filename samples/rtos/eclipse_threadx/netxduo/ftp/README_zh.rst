.. _netx_duo_ftp_server:

Netx Duo Ftp服务器
==============================

概述
------

这个例程演示了netx的ftp服务器功能。它实现了一个ftp服务器。

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
当访问ftp服务器时,默认的用户名是“name”,默认的密码是“password”,确认后ftp客户端就可以创建、上传、下载、删除服务器端的文件了。需要注意的是，ftp服务器的容量是有限的，默认是128kB。


.. code-block:: console

   FTP Server create success, available storage area is 1310720 bytes
   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   Server started!
   Logged in!

