.. _lwip_ptp_v1_master:

lwip_ptp_v1_master
====================================

概述
------

本示例展示PTP V1 Master的功能

- Master与Slave通过PTP协议通讯，从而实现Slave端的时钟同步

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接开发板RGMII或者RMII端口和另一个块开发板的RGMII或RMII端口

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

     This is an ethernet demo: PTP V1 Master
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     (D 1651074124.010753180) event POWER UP
     (D 1651074124.014336740) initClock
     (D 1651074124.017499900) state PTP_INITIALIZING
     (D 1651074124.021785540) manufacturerIdentity: PTPd;1.0.1
     (D 1651074124.026943940) netShutdown
     (D 1651074124.030266580) netInit
     (D 1651074124.033355900) initData
     (D 1651074124.036429900) initTimer
     (D 1651074124.039572540) initClock
     (D 1651074124.042730740) sync message interval: 1000
     (D 1651074124.047452300) clock identifier: DFLT
     (D 1651074124.051735060) 256*log2(clock variance): -4000
     (D 1651074124.056798180) clock stratum: 4
     (D 1651074124.060553660) clock preferred?: no
     (D 1651074124.064655900) bound interface name:
     (D 1651074124.068934340) communication technology: 1
     (D 1651074124.073648380) uuid: cc:00:00:00:80:85
     (D 1651074124.078024060) PTP subdomain name: _DFLT
     (D 1651074124.082563780) subdomain address: 224.0.1.129
     (D 1651074124.087540660) event port address: 3f 1
     (D 1651074124.091993740) general port address: 40 1
     (D 1651074124.096623660) initClock
     (D 1651074124.099763180) state PTP_LISTENING
     (D 1651074124.103781220) initClock
     (D 1651074124.106920900) state PTP_MASTER


