.. _lwip_tcpecho_multi_ports:

lwip_tcpecho_multi_ports
================================================

概述
------

本示例展示多端口TCP回送通讯

- PC 通过以太网发送TCP数据帧至MCU，MCU将接收的数据帧回发至PC

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用两根以太网线缆连接PC以太网端口和开发板上RGMII端口和RMII端口

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

     This is an ethernet demo: TCP Echo On Multiple Ports (Polling Usage)
     LwIP Version: 2.1.2
     Enet1 Reference Clock: Internal Clock
     Enet0 init passed!
     Enet1 init passed!
     ================ Network Interface 1 ================
     Link Status: Up
     Link Speed:  100Mbps
     Link Duplex: Full duplex
     ================ Network Interface 1 ================
     IPv4 Address: 192.168.200.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.200.1
     ================ Network Interface 0 ================
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     ================ Network Interface 0 ================
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1


* 打开以太网调试工具

  - 创建并设置TCP客户端：

    TCP服务端0 IP: 192.168.100.10/端口: 5001
    TCP服务端1 IP: 192.168.200.10/端口: 5002

    **注：实际需要根据PC所在局域网段调整服务端IP**

  - 连接

  - 在数据发送窗口(192.168.100.10/5001)编辑发送字符

    .. image:: ../doc/lwip_tcpecho_multi_ports_0_1.png

  - 观察回送数据窗口(192.168.100.10/5001)

    .. image:: ../doc/lwip_tcpecho_multi_ports_0_2.png

  - 在数据发送窗口(192.168.200.10/5002)编辑发送字符

    .. image:: ../doc/lwip_tcpecho_multi_ports_1_1.png

  - 观察回送数据窗口(192.168.200.10/5002)

    .. image:: ../doc/lwip_tcpecho_multi_ports_1_2.png