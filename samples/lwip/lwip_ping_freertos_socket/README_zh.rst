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

  - 在 ``CMakeLists.txt`` 中，若 CMake 变量 ``LWIP_DHCP`` 为 ``未定义``，则默认设为 ``0`` （静态 IP），并通过 ``sdk_compile_definitions(-DLWIP_DHCP=${LWIP_DHCP})`` 传入工程。

    - ``默认（静态 IP）`` ：在 ``netinfo.h`` 中配置 ``IP0_CONFIG`` / ``NETMASK0_CONFIG`` / ``GW0_CONFIG`` （或通过编译宏覆盖）。未使用 DHCP 时 DNS 默认为 114.114.114.114。

    - ``启用 DHCP`` ：配置工程时传入 ``-DLWIP_DHCP=1`` （例如 ``cmake -DLWIP_DHCP=1 ...``），或在 ``CMakeLists.txt`` 中于 ``if(NOT DEFINED LWIP_DHCP)`` 之前将 ``LWIP_DHCP`` 设为 1。

  - DNS 配置（静态 IP 模式，与默认工程一致时适用）:

    - 默认使用 114.114.114.114。若 MCU 能 ping 国内域名（如 baidu）但无法解析国际域名（如 www.st.com），而 PC 正常，多为 DNS 服务器不同所致。可在 ``lwipopts_app.h`` 中于 ``#if !LWIP_DHCP`` 前添加 ``#define DNS_SERVER_CONFIG 192.168.100.1`` （或网关 IP），改用网关作为 DNS，与 PC 使用相同解析路径。

运行示例
------------

* 编译下载程序

* ``默认编译`` （静态 IP，``LWIP_DHCP=0``）时串口终端示例如下。若使用 ``DHCP`` （``-DLWIP_DHCP=1``）编译，会先打印 ``DHCP State: ...`` 等行再打印 IPv4；地址以实际网络为准。


  .. code-block:: console

     This is an ethernet demo: Ping With Socket API On FreeRTOS
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
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


