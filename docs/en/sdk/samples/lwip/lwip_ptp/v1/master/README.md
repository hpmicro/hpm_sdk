# lwIP PTP Master V1

## Overview

This  example shows a functionality of PTP Master

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

