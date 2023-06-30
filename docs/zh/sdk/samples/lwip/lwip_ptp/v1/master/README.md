# lwIP PTP Master V1

## 概述

本示例展示PTP Master的功能

- Master与Slave通过PTP协议通讯，从而实现Slave端的时钟同步

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口
* 使用以太网线缆连接开发板RGMII或者RMII端口和另一个块开发板的RGMII或RMII端口

## 工程配置

- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(CONFIG_ENET_PHY_DP83848 1)"

## 运行示例

* 编译下载程序

* 串口终端显示如下信息：

  ```console
  This is an ethernet demo: PTP Master	LwIP Version: 2.1.2
  Enet phy init passes !
  Static IP: 10.10.10.10
  NETMASK  : 255.255.255.0
  Gateway  : 10.10.10.1
  (D 1651074120.006556560) event POWER UP
  (D 1651074120.010139640) initClock
  (D 1651074120.013296760) state PTP_INITIALIZING
  (D 1651074120.017579520) manufacturerIdentity: PTPd;1.0.1
  (D 1651074120.022731680) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.030851520) netInit
  (D 1651074120.033943160) initData
  (D 1651074120.037015520) initTimer
  (D 1651074120.040156280) initClock
  (D 1651074120.043313680) sync message interval: 1000
  (D 1651074120.048031080) clock identifier: DFLT
  (D 1651074120.052310560) 256*log2(clock variance): -4000
  (D 1651074120.057375680) clock stratum: 4
  (D 1651074120.061130600) clock preferred?: no
  (D 1651074120.065233280) bound interface name: 
  (D 1651074120.069511520) communication technology: 1
  (D 16510741
  20.074224160) uuid: 18:00:00:00:e0:a9
  (D 1651074120.078601240) PTP subdomain name: _DFLT
  (D 1651074120.083139440) subdomain address: 224.0.1.129
  (D 1651074120.088117520) event port address: 3f 1
  (D 1651074120.092568880) general port address: 40 1
  (D 1651074120.097198280) initClock
  (D 1651074120.100342920) state PTP_LISTENING
  (D 1651074120.104357200) initClock
  (D 1651074120.107499400) state PTP_MASTER

  Link Status: Down
  Link Status: Up
  Link Speed:  1000Mbps
  Link Duplex: Full duplex
  ```

  

