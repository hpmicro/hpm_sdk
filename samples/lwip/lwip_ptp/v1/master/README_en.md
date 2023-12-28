# lwip_ptp_v1_master

## Overview

This  example shows a functionality of PTP V1 Master

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
  This is an ethernet demo: PTP V1 Master
  LwIP Version: 2.1.2
  Enet phy init passed !
  Static IP: 192.168.100.10
  Netmask  : 255.255.255.0
  Gateway  : 192.168.100.1
  (D 1651074120.007092200) event POWER UP
  (D 1651074120.010675280) initClock
  (D 1651074120.013835040) state PTP_INITIALIZING
  (D 1651074120.018119000) manufacturerIdentity: PTPd;1.0.1
  (D 1651074120.023275360) netShutdown
  igmp_leavegroup: attempt to leave non-multicast address(D 1651074120.031401160) netInit
  (D 1651074120.034497160) initData
  (D 1651074120.037565760) initTimer
  (D 1651074120.040707000) initClock
  (D 1651074120.043862640) sync message interval: 1000
  (D 1651074120.048583440) clock identifier: DFLT
  (D 1651074120.052866840) 256*log2(clock variance): -4000
  (D 1651074120.057934720) clock stratum: 4
  (D 1651074120.061692400) clock preferred?: no
  (D 1651074120.065797840) bound interface name:
  (D 1651074120.070077760) communication technology: 1
  (D 1651074120.074793040) uuid: cc:00:00:00:80:85
  (D 1651074120.079173400) PTP subdomain name: _DFLT
  (D 1651074120.083713120) subdomain address: 224.0.1.129
  (D 1651074120.088690840) event port address: 3f 1
  (D 1651074120.093144400) general port address: 40 1
  (D 1651074120.097773640) initClock
  (D 1651074120.100913760) state PTP_LISTENING
  (D 1651074120.104934680) initClock
  (D 1651074120.108074560) state PTP_MASTER
Link Status: Up
Link Speed:  1000Mbps
  Link Duplex: Full duplex
  Link Status: Down
  ```
