# Netx Duo Ping

## Overview

The netx duo ping example shows netx arp and icmp functions. This example enables ping response function.

## Board Setting

No special settings are required

## Project Configuration
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number
- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Running the example

When the example runs successfully, it will print phy link status periodically until its status becomes Up, and it will print ICMP packets received count periodically. Following message is displayed in the terminal:
```console
NetXDuo is running
IP address: 192.168.1.223
Mask: 255.255.255.0
received 0 ICMP packets
Enet phy init passes !
received 0 ICMP packets
Link Status: Down
received 0 ICMP packets
received 0 ICMP packets
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 0 ICMP packets
received 1 ICMP packets
received 2 ICMP packets
received 3 ICMP packets
received 4 ICMP packets
```
