# lwip_ptp_v2_master

## 概述

本示例展示PTP V2 Master的功能

- Master与Slave通过PTP协议通讯，从而实现Slave端的时钟同步

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接开发板RGMII或者RMII端口和另一个块开发板的RGMII或RMII端口

## 工程配置

- 以太网端口配置：参考 [以太网通用工程设置](../../../doc/Ethernet_Common_Project_Settings_zh.md)
- 以太网DHCP配置
    - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:
      - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能
      - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能


## 运行示例

* 编译下载程序
* 串口终端显示如下信息：

  ```console
  This is an ethernet demo: PTP V2 Master
  LwIP Version: 2.1.2
  Enet phy init passed !
  Static IP: 192.168.100.10
  Netmask  : 255.255.255.0
  Gateway  : 192.168.100.1
  (D 1651074120.007102500) event POWER UP
  (D 1651074120.010687940) state PTP_INITIALIZING
  (D 1651074120.014967740) manufacturerIdentity: PTPd;2.0.1
  (D 1651074120.020130140) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.028251540) netInit
  (D 1651074120.031339180) initData
  (D 1651074120.034417820) initTimer
  (D 1651074120.037566500) initClock
  (D 1651074120.040746420) state PTP_SLAVE
  (D 1651074120.044416740) initClock
  (D 1651074120.047563940) state PTP_MASTER
  (D 1651074120.051320740) SYNC INTERVAL TIMER : 1000 

  Link Status: Up
  Link Speed:  1000Mbps
  Link Duplex: Full duplex
  ```
