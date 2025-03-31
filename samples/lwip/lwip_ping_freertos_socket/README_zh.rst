.. _lwip_ping_freertos_socket:

lwip_ping_freertos_socket
==================================================

概述
------

本示例展示FreeRTOS中使用ping

- MCU ping指定网络设备（输入IP或者URL）

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

     This is an ethernet demo: Ping With Socket API On FreeRTOS
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     DHCP State: SELECTING
     DHCP State: CHECKING
     DHCP State: BOUND
     IPv4 Address: 192.168.11.25
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.11.254
     Please enter a target name(IP or URL):



* 输入IP或者URL，并按回车确认


  .. code-block:: console

   www.hpmicro.com



* 查看log


  .. code-block:: console

   Pinging www.hpmicro.com [47.108.71.172] ..................
     from 47.108.71.172 bytes=60 icmp_seq=0 ttl=48 time=33 ms
     from 47.108.71.172 bytes=60 icmp_seq=1 ttl=48 time=33 ms
     from 47.108.71.172 bytes=60 icmp_seq=2 ttl=48 time=33 ms
     from 47.108.71.172 bytes=60 icmp_seq=3 ttl=48 time=33 ms


