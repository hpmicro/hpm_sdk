# lwIP PTP Slave V1

## 概述

本示例展示PTP Slave的功能

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
  This is an ethernet demo: PTP Slave
  LwIP Version: 2.1.2
  Reference Clock: Internal Clock
  Enet phy init passes !
  Static IP: 10.10.10.10
  NETMASK  : 255.255.255.0
  Gateway  : 10.10.10.1
  (D 1651074120.006541480) event POWER UP
  (D 1651074120.010119880) initClock
  (D 1651074120.013274690) state PTP_INITIALIZING
  (D 1651074120.015414430) manufacturerIdentity: PTPd;1.0.1
  (D 1651074120.017989770) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.022046910) netInit
  (D 1651074120.023591870) initData
  (D 1651074120.025128070) initTimer
  (D 1651074120.026697790) initClock
  (D 1651074120.028272730) sync message interval: 4000
  (D 1651074120.030630510) clock identifier: DFLT
  (D 1651074120.032767250) 256*log2(clock variance): -4000
  (D 1651074120.035295690) clock stratum: 255
  (D 1651074120.037257510) clock preferred?: no
  (D 1651074120.039306490) bound interface name: 
  (D 1651074120.041442750) communication technology: 1
  (D 1651074120.043796390) uuid: cc:8e:a0:4f:80:85
  (D 1651074120.045982630) PTP subdomain name: _DFLT
  (D 1651074120.048249970) subdomain address: 224.0.1.129
  (D 1651074120.050735330) event port address: 3f 1
  (D 1651074120.052958470) general port address: 40 1
  (D 1651074120.055272350) initClock
  (D 1651074120.056843470) state PTP_LISTENING
  (D 1651074120.058850730) initClock
  (D 1651074120.060421810) state PTP_SLAVE
  Link Status: Down
  Link Status: Down
  Link Status: Up
  Link Speed:  100Mbps
  Link Duplex: Full duplex
  (D 1651074121.652940650) addForeign: new record (0,1) 1 1 8b:81:55:23:30:1a
(D 1651074121.656315550) event MASTER_CLOCK_CHANGED
  (D 1651074121.658622410) state PTP_UNCALIBRATED
  (D 1651074121.660755730) toState: Q = 0, R = 6
  (D 1651074123.652889630) updateOffset
  (D 1651074123.654597110) updateClock seconds
  (D 1651074130.008827280) setTime: resetting system clock to 1651074130s 8827130ns
  (D 1651074130.012445980) initClock
  (D 1651074130.014017360) one-way delay:           0s          0ns
  (D 1651074130.016939600) offset from master:      -6s -352226480ns
  (D 1651074130.019904400) observed drift:          0
  (D 1651074132.005082640) updateOffset
  (D 1651074132.006796280) one-way delay:           0s          0ns
  (D 1651074132.009717430) offset from master:      0s     -29130ns
  (D 1651074132.012638150) observed drift:          -485
  (D 1651074134.005070290) updateOffset
  (D 1651074134.006776190) one-way delay:           0s          0ns
  (D 1651074134.009697270) offset from master:      0s     -33825ns
  (D 1651074134.012617900) observed drift:          -1048
  (D 1651074136.005060910) updateOffset
  (D 1651074136.006767760) one-way delay:           0s          0ns
  (D 1651074136.009688740) offset from master:      0s     -40303ns
  (D 1651074136.012609960) observed drift:          -1719
  (D 1651074138.005054100) updateOffset
  (D 1651074138.006759980) one-way delay:           0s          0ns
  (D 1651074138.009680640) offset from master:      0s     -46726ns
  (D 1651074138.012601220) observed drift:          -2497
  (D 1651074140.005045660) handleSync: Q = 0, R = 16
  (D 1651074140.007326080) updateOffset
  (D 1651074140.009032970) one-way delay:           0s          0ns
  (D 1651074140.011954400) offset from master:      0s     -52188ns
  (D 1651074140.014875190) observed drift:          -3366
  (D 1651074142.005050730) updateOffset
  (D 1651074142.006757600) one-way delay:           0s          0ns
  (D 1651074142.009680800) offset from master:      0s     -53399ns
  (D 1651074142.012601190) observed drift:          -4256
  (D 1651074144.005051810) updateOffset
  (D 1651074144.006757660) one-way delay:           0s          0ns
  (D 1651074144.009678840) offset from master:      0s     -53055ns
  (D 1651074144.012599750) observed drift:          -5140
  (D 1651074146.005058230) updateOffset
  (D 1651074146.006764010) one-way delay:           0s          0ns
  (D 1651074146.009684940) offset from master:      0s     -51282ns
  (D 1651074146.012605820) observed drift:          -5994
  (D 1651074148.005063450) updateOffset
  (D 1651074148.006769230) one-way delay:           0s          0ns
  (D 1651074148.009690130) offset from master:      0s     -47436ns
  (D 1651074148.012611230) observed drift:          -6784
  (D 1651074150.005069460) updateOffset
  (D 1651074150.006774740) one-way delay:           0s          0ns
  (D 1651074150.009695800) offset from master:      0s     -41628ns
  (D 1651074150.012616590) observed drift:          -7477
  (D 1651074152.005075700) updateOffset
  (D 1651074152.006782280) one-way delay:           0s          0ns
  (D 1651074152.009703210) offset from master:      0s     -35779ns
  (D 1651074152.012623950) observed drift:          -8073
  (D 1651074154.005080520) updateOffset
  (D 1651074154.006785430) one-way delay:           0s          0ns
  (D 1651074154.009706120) offset from master:      0s     -30140ns
  (D 1651074154.012626660) observed drift:          -8575
  (D 1651074156.005086990) updateOffset
  (D 1651074156.006793010) one-way delay:           0s          0ns
  (D 1651074156.009714580) offset from master:      0s     -24095ns
  (D 1651074156.012635340) observed drift:          -8976
  (D 1651074158.005089920) updateOffset
  (D 1651074158.006797020) one-way delay:           0s          0ns
  (D 1651074158.009718580) offset from master:      0s     -18327ns
  (D 1651074158.012639110) observed drift:          -9281
  (D 1651074160.005095910) updateOffset
  (D 1651074160.006802530) one-way delay:           0s          0ns
  (D 1651074160.009724300) offset from master:      0s     -13339ns
  (D 1651074160.012645010) observed drift:          -9503
  (D 1651074162.005099020) updateOffset
  (D 1651074162.006806760) one-way delay:           0s          0ns
  (D 1651074162.009729010) offset from master:      0s      -8879ns
  (D 1651074162.012649850) observed drift:          -9651
  (D 1651074162.015136810) event MASTER_CLOCK_SELECTED
  (D 1651074162.017485440) state PTP_SLAVE
  (D 1651074164.005105050) updateOffset
  (D 1651074164.006813000) one-way delay:           0s          0ns
  (D 1651074164.009735050) offset from master:      0s      -4330ns
  (D 1651074164.012655920) observed drift:          -9723
  (D 1651074166.005100390) updateOffset
  (D 1651074166.006806390) one-way delay:           0s          0ns
  (D 1651074166.009728480) offset from master:      0s      -1875ns
  (D 1651074166.012649390) observed drift:          -9754
  (D 1651074168.005101090) updateOffset
  (D 1651074168.006807670) one-way delay:           0s          0ns
  (D 1651074168.009732510) offset from master:      0s      -1187ns
  (D 1651074168.012653180) observed drift:          -9773
  ```
  
  

