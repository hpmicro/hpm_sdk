# Netx Duo Udp

## Overview

The netx duo udp example shows netx udp send and receive functions.

## Board Setting

No special settings are required

## Project Configuration
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number
- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Running the example

When the example runs successfully, it will send udp message **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "** to **ECHO_SERVER_ADDRESS : ECHO_SERVER_PORT**(192.168.1.2:7777) through port **ECHO_SEND_PORT**(7777) and then wait for receive udp message with a timeout. It will print phy link status periodically until its status becomes **Up**. If received udp message, it will also print it to the console. Following message is displayed in the terminal:
```console
NetXDuo is running
IP address: 192.168.1.223
Mask: 255.255.255.0
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
No response from server 1
No response from server 2
No response from server 3
No response from server 4
Receive response: abcdefg 5
Receive response: abcdefg 6
Receive response: abcdefg 7
Receive response: abcdefg 8
Receive response: abcdefg 9
Receive response: abcdefg 10
Receive response: abcdefg 11
```
