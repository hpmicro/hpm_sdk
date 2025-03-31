.. _lwip_http_server:

lwip_http_server
================================

概述
------

本示例演示使用lwip http实现网页服务器的功能

注：

* 重复刷新网页，可能会出现异常

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口

工程配置
------------

- 以太网端口配置：参考 :ref:`以太网通用工程设置 <ethernet_port_configurations_zh>`

- 以太网DHCP配置

  - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:

    - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能

    - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


  .. code-block:: console

   This is an ethernet demo: HTTP Server (Polling Usage)
   LwIP Version: 2.1.2
   Enet phy init passed !
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   IPv4 Address: 192.168.100.10
   IPv4 Netmask: 255.255.255.0
   IPv4 Gateway: 192.168.100.1


*  使用浏览器客户端，输入http://192.168.100.10，回车后显示如下的页面：

   注：此示例可以成功运行，通过使用360或IE内核的浏览器

   .. image:: doc/lwip_httpsrv.png
      :alt: https_server
