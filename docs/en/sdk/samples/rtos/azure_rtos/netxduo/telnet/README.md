# Netx Duo Telnet

## Overview

The netx duo telnet example shows netx telnet server functions. This example enables a telnet server.

## Board Setting

No special settings are required

## Project Configuration
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number
- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Running the example

When the example runs successfully, it will print phy link status periodically until its status becomes Up. Following message is displayed in the terminal:
```console
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
```

When telnet client connect to the host, type a character in the client, the character will echo in the telnet terminal, specially enter a **q** will cause server close the connection. When enter a **Enter** key, it will turn to a new line.
