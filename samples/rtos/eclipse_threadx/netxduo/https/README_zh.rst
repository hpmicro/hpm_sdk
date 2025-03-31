.. _netx_duo_https_server:

Netx Duo Https Server
==========================================

概述
------

这个例程演示了netx的https服务器功能。它实现了一个https服务器。

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
当访问https服务器时,一个窗口会弹出,要求输入用户名和密码。默认的用户名是“name”,默认的密码是“password”,确认后会显示页面。


.. code-block:: text

   Test response from server



.. code-block:: console

   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex

