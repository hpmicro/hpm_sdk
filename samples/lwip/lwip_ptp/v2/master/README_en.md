# lwip_ptp_v2_master

## Overview

This example shows a functionality of PTP V2 Master

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
