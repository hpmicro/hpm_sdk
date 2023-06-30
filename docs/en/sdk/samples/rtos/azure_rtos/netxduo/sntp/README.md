# Netx Duo Sntp Client

## Overview

The netx duo sntp client example shows netx sntp client functions. This example enables a sntp client.

## Board Setting

No special settings are required

## Project Configuration
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number
- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Running the example

When the example runs successfully, it will print phy link status periodically until its status becomes Up. Following message is displayed in the terminal:
The program will request time data in a fixed point manner, and when the time is obtained, it will be printed to the terminal. Note: The routine requires that the root partition of the sntp server cannot be greater than 5 seconds.

```console
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
year [2023] :month [5]: day [5] :hour  [11]: min  [21]: second  [51]
SNTP updated
Time: 3892245711.435 sec.
```
