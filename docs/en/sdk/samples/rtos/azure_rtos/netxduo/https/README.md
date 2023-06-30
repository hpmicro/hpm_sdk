# Netx Duo Https Server

## Overview

The netx duo https server example shows netx https server functions. This example enables a https server.

## Board Setting

No special settings are required

## Project Configuration
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number
- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Running the example

When the example runs successfully, it will print phy link status periodically until its status becomes Up. Following message is displayed in the terminal:
When accessing the HTTPs server, a window will pop up asking for a username and password. The default username is "name" and the default password is "password". After confirmation, the page will be displayed.

```
Test response from server
```

```console
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
```
