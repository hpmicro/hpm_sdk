# lwIP PTP Slave V1

## Overview

This  example shows a functionality of PTP Slave

- Master communicates with slave via the PTP protocol.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable
- Connect an Ethernet RGMII port or RMII port on a development board with an Ethernet RGMII port or RMII port on another development board

## Project Configuration

- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Run Example

- Compiling and Downloading
- Running log is shown in the serial terminal as follows:

  ```console
  This is an ethernet demo: PTP Slave
  LwIP Version: 2.1.2
  Enet phy init passes !
  Static IP: 10.10.10.10
  NETMASK  : 255.255.255.0
  Gateway  : 10.10.10.1
  (D 1651074120.006559180) event POWER UP
  (D 1651074120.010144100) initClock
  (D 1651074120.013303260) state PTP_INITIALIZING
  (D 1651074120.017585900) manufacturerIdentity: PTPd;1.0.1
  (D 1651074120.022740100) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.030860700) netInit
  (D 1651074120.033946740) initData
  (D 1651074120.037019740) initTimer
  (D 1651074120.040163460) initClock
  (D 1651074120.043316540) sync message interval: 1000
  (D 1651074120.048035780) clock identifier: DFLT
  (D 1651074120.052314180) 256*log2(clock variance): -4000
  (D 1651074120.057376460) clock stratum: 255
  (D 1651074120.061307100) clock preferred?: no
  (D 1651074120.065410180) bound interface name: 
  (D 1651074120.069687500) communication technology: 1
  (D 1651074
  120.074400780) uuid: cc:00:00:00:80:85
  (D 1651074120.078777940) PTP subdomain name: _DFLT
  (D 1651074120.083317300) subdomain address: 224.0.1.129
  (D 1651074120.088295220) event port address: 3f 1
  (D 1651074120.092747020) general port address: 40 1
  (D 1651074120.097375980) initClock
  (D 1651074120.100517660) state PTP_LISTENING
  (D 1651074120.104530100) initClock
  (D 1651074120.107670780) state PTP_SLAVE
  
  Link Status: Down
  
  Link Status: Up
  Link Speed:  1000Mbps
  Link Duplex: Full duplex
  
  (D 1651074123.260799380) addForeign: new record (0,1) 1 1 18:00:00:00:e0:a9
  (D 1651074123.267590060) event MASTER_CLOCK_CHANGED
  (D 1651074123.272209900) state PTP_UNCALIBRATED
  (D 1651074123.276478100) toState: Q = 0, R = 6
  
  (D 1651074124.254980820) updateOffset
  (D 1651074124.258393820) updateClock seconds
  (D 1651074139.004759360) setTime: resetting system clock to 1651074139s 4759060ns
  (D 1651074139.012006680) initClock
  (D 1651074139.015155000) one-way delay:           0s          0ns
  (D 1651074139.021003440) offset from master:      -14s -742360880ns
  (D 1651074139.027026360) observed drift:          0
  
  (D 1651074139.997323560) updateOffset
  (D 1651074140.000754960) one-way delay:           0s          0ns
  (D 1651074140.006605600) offset from master:      0s      -4760ns
  (D 1651074140.012453500) observed drift:          -317
  
  (D 1651074140.997319440) updateOffset
  (D 1651074141.000736880) one-way delay:           0s          0ns
  (D 1651074141.006585860) offset from master:      0s      -5370ns
  (D 1651074141.012433680) observed drift:          -675
  
  (D 1651074141.997316880) updateOffset
  (D 1651074142.000730880) one-way delay:           0s          0ns
  (D 1651074142.006579880) offset from master:      0s      -5965ns
  (D 1651074142.012427780) observed drift:          -1072
  
  (D 1651074142.997307080) updateOffset
  (D 1651074143.000721020) one-way delay:           0s          0ns
  (D 1651074143.006568800) offset from master:      0s      -6203ns
  (D 1651074143.012416760) observed drift:          -1485
  
  (D 1651074143.997284580) handleSync: Q = 0, R = 16
  (D 1651074144.001866420) updateOffset
  (D 1651074144.005273740) one-way delay:           0s          0ns
  (D 1651074144.011121620) offset from master:      0s      -5991ns
  (D 1651074144.016969680) observed drift:          -1884
  
  (D 1651074144.997312500) updateOffset
  (D 1651074145.000727640) one-way delay:           0s          0ns
  (D 1651074145.006575360) offset from master:      0s      -5416ns
  (D 1651074145.012423220) observed drift:          -2245
  
  (D 1651074145.997312300) updateOffset
  (D 1651074146.000722400) one-way delay:           0s          0ns
  (D 1651074146.006570140) offset from master:      0s      -4628ns
  (D 1651074146.012418100) observed drift:          -2553
  
  (D 1651074146.997314200) updateOffset
  (D 1651074147.000725300) one-way delay:           0s          0ns
  (D 1651074147.006572920) offset from master:      0s      -3774ns
  (D 1651074147.012420880) observed drift:          -2804
  
  (D 1651074147.997321620) updateOffset
  (D 1651074148.000731680) one-way delay:           0s          0ns
  (D 1651074148.006579500) offset from master:      0s      -2977ns
  (D 1651074148.012427360) observed drift:          -3002
  
  (D 1651074148.997306900) updateOffset
  (D 1651074149.000715880) one-way delay:           0s          0ns
  (D 1651074149.006563540) offset from master:      0s      -2308ns
  (D 1651074149.012411380) observed drift:          -3155
  
  (D 1651074149.997309900) updateOffset
  (D 1651074150.000721800) one-way delay:           0s          0ns
  (D 1651074150.006569460) offset from master:      0s      -1804ns
  (D 1651074150.012417200) observed drift:          -3275
  
  (D 1651074150.997313280) updateOffset
  (D 1651074151.000721880) one-way delay:           0s          0ns
  (D 1651074151.006569560) offset from master:      0s      -1442ns
  (D 1651074151.012417420) observed drift:          -3371
  
  (D 1651074151.997306800) updateOffset
  (D 1651074152.000717780) one-way delay:           0s          0ns
  (D 1651074152.006565480) offset from master:      0s      -1201ns
  (D 1651074152.012413340) observed drift:          -3451
  
  (D 1651074152.997307620) updateOffset
  (D 1651074153.000719320) one-way delay:           0s          0ns
  (D 1651074153.006567060) offset from master:      0s      -1051ns
  (D 1651074153.012414840) observed drift:          -3521
  
  (D 1651074153.997310140) updateOffset
  (D 1651074154.000723200) one-way delay:           0s          0ns
  (D 1651074154.006570780) offset from master:      0s       -945ns
  (D 1651074154.012418480) observed drift:          -3584
  
  (D 1651074154.997311360) updateOffset
  (D 1651074155.000722020) one-way delay:           0s          0ns
  (D 1651074155.006569920) offset from master:      0s       -853ns
  (D 1651074155.012417700) observed drift:          -3640
  
  (D 1651074155.997312140) updateOffset
  (D 1651074156.000723760) one-way delay:           0s          0ns
  (D 1651074156.006571340) offset from master:      0s       -776ns
  (D 1651074156.012419120) observed drift:          -3691
  
  (D 1651074156.997310620) updateOffset
  (D 1651074157.000722000) one-way delay:           0s          0ns
  (D 1651074157.006569700) offset from master:      0s       -688ns
  (D 1651074157.012417440) observed drift:          -3736
  
  (D 1651074157.997317340) updateOffset
  (D 1651074158.000728240) one-way delay:           0s          0ns
  (D 1651074158.006576100) offset from master:      0s       -604ns
  (D 1651074158.012423960) observed drift:          -3776
  
  (D 1651074158.997315100) updateOffset
  (D 1651074159.000725880) one-way delay:           0s          0ns
  (D 1651074159.006573660) offset from master:      0s       -532ns
  (D 1651074159.012421600) observed drift:          -3811
  
  (D 1651074159.997290940) handleSync: Q = 0, R = 16
  (D 1651074160.001867280) updateOffset
  (D 1651074160.005271840) one-way delay:           0s          0ns
  (D 1651074160.011120920) offset from master:      0s       -456ns
  (D 1651074160.016968860) observed drift:          -3841
  
  (D 1651074160.997325520) updateOffset
  (D 1651074161.000738700) one-way delay:           0s          0ns
  (D 1651074161.006587560) offset from master:      0s       -388ns
  (D 1651074161.012435420) observed drift:          -3866
  
  (D 1651074161.997311540) updateOffset
  (D 1651074162.000726000) one-way delay:           0s          0ns
  (D 1651074162.006574460) offset from master:      0s       -344ns
  (D 1651074162.012422400) observed drift:          -3888
  
  (D 1651074162.997319840) updateOffset
  (D 1651074163.000733260) one-way delay:           0s          0ns
  (D 1651074163.006580960) offset from master:      0s       -302ns
  (D 1651074163.012428620) observed drift:          -3908
  
  (D 1651074163.997315860) updateOffset
  (D 1651074164.000726960) one-way delay:           0s          0ns
  (D 1651074164.006574540) offset from master:      0s       -261ns
  (D 1651074164.012422240) observed drift:          -3925
  
  (D 1651074164.997313960) updateOffset
  (D 1651074165.000726820) one-way delay:           0s          0ns
  (D 1651074165.006574520) offset from master:      0s       -241ns
  (D 1651074165.012422300) observed drift:          -3941
  
  (D 1651074165.997318180) updateOffset
  (D 1651074166.000731560) one-way delay:           0s          0ns
  (D 1651074166.006579180) offset from master:      0s       -220ns
  (D 1651074166.012426960) observed drift:          -3955
  
  (D 1651074166.997314400) updateOffset
  (D 1651074167.000729940) one-way delay:           0s          0ns
  (D 1651074167.006577680) offset from master:      0s       -190ns
  (D 1651074167.012425700) observed drift:          -3967
  
  (D 1651074167.997311140) updateOffset
  (D 1651074168.000724360) one-way delay:           0s          0ns
  (D 1651074168.006572100) offset from master:      0s       -175ns
  (D 1651074168.012420120) observed drift:          -3978
  
  (D 1651074168.997318940) updateOffset
  (D 1651074169.000734580) one-way delay:           0s          0ns
  (D 1651074169.006582280) offset from master:      0s       -158ns
  (D 1651074169.012430200) observed drift:          -3988
  
  (D 1651074169.997314600) updateOffset
  (D 1651074170.000731780) one-way delay:           0s          0ns
  (D 1651074170.006579560) offset from master:      0s       -149ns
  (D 1651074170.012427420) observed drift:          -3997
   ```

