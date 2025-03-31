.. _lwip_tcpclient_freertos_socket:

lwip_tcpclient_freertos_socket
============================================================

概述
------

本示例展示FreeRTOS中TCP客户端用法

- MCU作为TCP客户端周期向服务端发送数据帧

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

     This is an ethernet demo: TCP Client With Socket API On FreeRTOS
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     Connect to server successfully!


* 打开以太网调试工具

  - 创建并设置TCP服务端：TCP服务端IP: 192.168.100.5/端口: 5001

  - 打开

  - 在数据接收窗口观察接收数据

    .. image:: ../doc/lwip_tcpclient.png
