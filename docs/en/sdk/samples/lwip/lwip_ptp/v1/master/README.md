# lwIP PTP Master V1

## Overview

This  example shows a functionality of PTP Master 

- Master communicates with slave via the PTP protocol.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable
- Connect an Ethernet RGMII port or RMII port on a development board with an Ethernet RGMII port or RMII port on another development board

## Project Configuration

- In the file `CMakelists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(COFNIG_ENET_PHY_DP83848 1)"

## Run Example

- Compiling and Downloading
- Running log is shown in the serial terminal as follows:

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

