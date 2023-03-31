# lwIP PTP Master V1

## 概述

本示例展示PTP Master的功能

- Master与Slave通过PTP协议通讯，从而实现Slave端的时钟同步

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口
* 使用以太网线缆连接开发板RGMII或者RMII端口和另一个块开发板的RGMII或RMII端口

## 工程配置

- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(COFNIG_ENET_PHY_DP83848 1)"

## 运行示例

* 编译下载程序

* 串口终端显示如下信息：

  ```console
  This is an ethernet demo: PTP Master
  LwIP Version: 2.1.2
  Reference Clock: Internal Clock
  Enet phy init passes !
  Static IP: 10.10.10.10
  NETMASK  : 255.255.255.0
  Gateway  : 10.10.10.1
  (D 1651074120.006539780) event POWER UP
  (D 1651074120.010118460) initClock
  (D 1651074120.013270990) state PTP_INITIALIZING
  (D 1651074120.015411190) manufacturerIdentity: PTPd;1.0.1
  (D 1651074120.017987490) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.022044630) netInit
  (D 1651074120.023586230) initData
  (D 1651074120.025121190) initTimer
  (D 1651074120.026690130) initClock
  (D 1651074120.028267670) sync message interval: 4000
  (D 1651074120.030627290) clock identifier: DFLT
  (D 1651074120.032764310) 256*log2(clock variance): -4000
  (D 1651074120.035292930) clock stratum: 4
  (D 1651074120.037167730) clock preferred?: no
  (D 1651074120.039216690) bound interface name: 
  (D 1651074120.041352590) communication technology: 1
  (D 1651074120.043706630) uuid: 8b:81:55:23:30:1a
  (D 1651074120.045893210) PTP subdomain name: _DFLT
  (D 1651074120.048160890) subdomain address: 224.0.1.129
  (D 1651074120.050646490) event port address: 3f 1
  (D 1651074120.052870310) general port address: 40 1
  (D 1651074120.055181630) initClock
  (D 1651074120.056751370) state PTP_LISTENING
  (D 1651074120.058757190) initClock
  (D 1651074120.060326150) state PTP_MASTER
  Link Status: Down
  Link Status: Down
  Link Status: Up
  Link Speed:  100Mbps
  Link Duplex: Full duplex
  ```

  

