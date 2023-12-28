# lwip_ptp_v2_slave

## Overview

This example shows a functionality of PTP V2 Slave

- Master communicates with slave via the PTP protocol.

## Board Settings

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect an Ethernet RGMII port or RMII port on a development board with an Ethernet RGMII port or RMII port on another development board

## Project Configurations

- Ethernet Port Settings: Refer to [Ethernet common project settings](../../../doc/Ethernet_Common_Project_Settings_en.md)
- Ethernet DHCP Configurations
    - the configurations in the `CMakeLists.txt` as follows:
      - sdk_compile_definitions(-DLWIP_DHCP=0): Disable DHCP feature
      - sdk_compile_definitions(-DLWIP_DHCP=1): Enable DHCP feature


## Run Example

- Compiling and Downloading
- Running log is shown in the serial terminal as follows:

  ```console
  This is an ethernet demo: PTP V2 Slave
  LwIP Version: 2.1.2
  Enet phy init passed !
  Static IP: 192.168.100.11
  Netmask  : 255.255.255.0
  Gateway  : 192.168.100.1
  (D 1651074120.007097640) event POWER UP
  (D 1651074120.010681920) state PTP_INITIALIZING
  (D 1651074120.014962920) manufacturerIdentity: PTPd;2.0.1
  (D 1651074120.020123400) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.028250320) netInit
  (D 1651074120.031337160) initData
  (D 1651074120.034424240) initTimer
  (D 1651074120.037570520) initClock
  (D 1651074120.040750120) state PTP_SLAVE
  (D 1651074120.044419440) state PTP_SLAVE
  Link Status: Down
  Link Status: Down
  Link Status: Up
  Link Speed:  1000Mbps
  Link Duplex: Full duplex
  (D 1651074146.359307200) event MASTER_CLOCK_CHANGED
  (D 1651074146.363948320) state PTP_UNCALIBRATED
  (D 1651074125.002284680) setTime: resetting system clock to 1651074125s 2284380ns
  (D 1651074125.009556480) initClock
  (D 1651074125.012711000) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074125.021096960) updateClock: offset from master:              22s   356060980ns
  (D 1651074125.028964640) updateClock: observed drift:                   0
  (D 1651074126.002251920) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074126.010658320) updateClock: offset from master:               0s      -58100ns
  (D 1651074126.018517620) updateClock: observed drift:               -3873
  (D 1651074127.002165320) updateClock: one-way delay averaged (P2P):           0s           0ns
(D 1651074127.010559480) updateClock: offset from master:               0s      -68380ns
  (D 1651074127.018414380) updateClock: observed drift:               -8431
(D 1651074128.002151620) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074128.010555380) updateClock: offset from master:               0s      -78930ns
  (D 1651074128.018419560) updateClock: observed drift:              -13693
  (D 1651074129.002158420) updateClock: one-way delay averaged (P2P):           0s           0ns
(D 1651074129.010549820) updateClock: offset from master:               0s      -84355ns
  (D 1651074129.018405740) updateClock: observed drift:              -19316
  (D 1651074130.002171400) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074130.010565940) updateClock: offset from master:               0s      -83028ns
  (D 1651074130.018430640) updateClock: observed drift:              -24851
(D 1651074131.002168400) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074131.010572280) updateClock: offset from master:               0s      -75884ns
  (D 1651074131.018433340) updateClock: observed drift:              -29909
  (D 1651074132.002181120) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074132.010576840) updateClock: offset from master:               0s      -65082ns
  (D 1651074132.018438180) updateClock: observed drift:              -34247
  (D 1651074133.002199800) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074133.010602900) updateClock: offset from master:               0s      -52981ns
(D 1651074133.018460400) updateClock: observed drift:              -37779
  (D 1651074134.002191060) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074134.010589540) updateClock: offset from master:               0s      -41480ns
  (D 1651074134.018445700) updateClock: observed drift:              -40544
  (D 1651074135.002206620) updateClock: one-way delay averaged (P2P):           0s           0ns
(D 1651074135.010605600) updateClock: offset from master:               0s      -31770ns
  (D 1651074135.018463140) updateClock: observed drift:              -42662
  (D 1651074136.002221380) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074136.010617220) updateClock: offset from master:               0s      -24315ns
  (D 1651074136.018476140) updateClock: observed drift:              -44283
(D 1651074137.002220860) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074137.010625280) updateClock: offset from master:               0s      -19028ns
  (D 1651074137.018485760) updateClock: observed drift:              -45551
  (D 1651074138.002220360) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074138.010620460) updateClock: offset from master:               0s      -15514ns
(D 1651074138.018477360) updateClock: observed drift:              -46585
  (D 1651074139.002223420) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074139.010634520) updateClock: offset from master:               0s      -13237ns
  (D 1651074139.018496900) updateClock: observed drift:              -47467
  (D 1651074140.002235300) updateClock: one-way delay averaged (P2P):           0s           0ns
(D 1651074140.010636560) updateClock: offset from master:               0s      -11708ns
  (D 1651074140.018493860) updateClock: observed drift:              -48247
  (D 1651074141.002224400) updateClock: one-way delay averaged (P2P):           0s           0ns
  (D 1651074141.010617540) updateClock: offset from master:               0s      -10534ns
  (D 1651074141.018477220) updateClock: observed drift:              -48949
  (D 1651074142.002211660) updateClock: one-way delay averaged (P2P):           0s           0ns
(D 1651074142.010608680) updateClock: offset from master:               0s       -9477ns
  (D 1651074142.018465420) updateClock: observed drift:              -49580
  (D 1651074142.025084060) event MASTER_CLOCK_SELECTED
  (D 1651074142.029805560) state PTP_SLAVE
  (D 1651074143.002262140) updateClock: one-way delay averaged (P2P):           0s         570ns
(D 1651074143.010658600) updateClock: offset from master:               0s       -8704ns
  (D 1651074143.018515120) updateClock: observed drift:              -50160
  (D 1651074144.002239540) updateClock: one-way delay averaged (P2P):           0s         570ns
  (D 1651074144.010631400) updateClock: offset from master:               0s       -7687ns
  (D 1651074144.018488060) updateClock: observed drift:              -50672
(D 1651074145.002259860) updateClock: one-way delay averaged (P2P):           0s         570ns
  (D 1651074145.010662460) updateClock: offset from master:               0s       -6558ns
  (D 1651074145.018523620) updateClock: observed drift:              -51109
  (D 1651074146.002261040) updateClock: one-way delay averaged (P2P):           0s         570ns
  (D 1651074146.010662100) updateClock: offset from master:               0s       -5414ns
(D 1651074146.018520120) updateClock: observed drift:              -51469
  (D 1651074147.002263260) updateClock: one-way delay averaged (P2P):           0s         560ns
  (D 1651074147.010672620) updateClock: offset from master:               0s       -4367ns
  (D 1651074147.018527660) updateClock: observed drift:              -51760
  (D 1651074148.002272440) updateClock: one-way delay averaged (P2P):           0s         560ns
(D 1651074148.010674640) updateClock: offset from master:               0s       -3484ns
  (D 1651074148.018534160) updateClock: observed drift:              -51992
  (D 1651074149.002253900) updateClock: one-way delay averaged (P2P):           0s         560ns
  (D 1651074149.010647420) updateClock: offset from master:               0s       -2782ns
  (D 1651074149.018503240) updateClock: observed drift:              -52177
(D 1651074150.002274500) updateClock: one-way delay averaged (P2P):           0s         560ns
  (D 1651074150.010676700) updateClock: offset from master:               0s       -2251ns
  (D 1651074150.018539800) updateClock: observed drift:              -52327
  (D 1651074151.002249800) updateClock: one-way delay averaged (P2P):           0s         560ns
  (D 1651074151.010648160) updateClock: offset from master:               0s       -1855ns
(D 1651074151.018505340) updateClock: observed drift:              -52450
  (D 1651074152.002279280) updateClock: one-way delay averaged (P2P):           0s         562ns
  (D 1651074152.010679420) updateClock: offset from master:               0s       -1569ns
  (D 1651074152.018536480) updateClock: observed drift:              -52554
  (D 1651074153.002262820) updateClock: one-way delay averaged (P2P):           0s         564ns
(D 1651074153.010663600) updateClock: offset from master:               0s       -1326ns
  (D 1651074153.018522580) updateClock: observed drift:              -52642
  (D 1651074154.002272340) updateClock: one-way delay averaged (P2P):           0s         564ns
  (D 1651074154.010670180) updateClock: offset from master:               0s       -1145ns
  (D 1651074154.018527600) updateClock: observed drift:              -52718
(D 1651074155.002245100) updateClock: one-way delay averaged (P2P):           0s         566ns
  (D 1651074155.010643220) updateClock: offset from master:               0s        -986ns
  (D 1651074155.018499960) updateClock: observed drift:              -52783
  (D 1651074156.002256980) updateClock: one-way delay averaged (P2P):           0s         565ns
  (D 1651074156.010656460) updateClock: offset from master:               0s        -835ns
(D 1651074156.018515920) updateClock: observed drift:              -52838
  (D 1651074157.002237180) updateClock: one-way delay averaged (P2P):           0s         565ns
  (D 1651074157.010639160) updateClock: offset from master:               0s        -710ns
  (D 1651074157.018496940) updateClock: observed drift:              -52885
  (D 1651074158.002226580) updateClock: one-way delay averaged (P2P):           0s         565ns
(D 1651074158.010617000) updateClock: offset from master:               0s        -598ns
  (D 1651074158.018473780) updateClock: observed drift:              -52924
  (D 1651074159.002241060) updateClock: one-way delay averaged (P2P):           0s         566ns
  (D 1651074159.010637780) updateClock: offset from master:               0s        -482ns
  (D 1651074159.018494720) updateClock: observed drift:              -52956
(D 1651074160.002268140) updateClock: one-way delay averaged (P2P):           0s         566ns
  (D 1651074160.010660700) updateClock: offset from master:               0s        -394ns
  (D 1651074160.018519120) updateClock: observed drift:              -52982
  (D 1651074161.002243540) updateClock: one-way delay averaged (P2P):           0s         567ns
  (D 1651074161.010643280) updateClock: offset from master:               0s        -310ns
(D 1651074161.018497580) updateClock: observed drift:              -53002
  (D 1651074162.002268100) updateClock: one-way delay averaged (P2P):           0s         567ns
  (D 1651074162.010659960) updateClock: offset from master:               0s        -239ns
  (D 1651074162.018516320) updateClock: observed drift:              -53017
  (D 1651074163.002263280) updateClock: one-way delay averaged (P2P):           0s         567ns
(D 1651074163.010660120) updateClock: offset from master:               0s        -193ns
  (D 1651074163.018515260) updateClock: observed drift:              -53029
  (D 1651074164.002255280) updateClock: one-way delay averaged (P2P):           0s         567ns
  (D 1651074164.010646140) updateClock: offset from master:               0s        -150ns
  (D 1651074164.018502980) updateClock: observed drift:              -53039

  ```
