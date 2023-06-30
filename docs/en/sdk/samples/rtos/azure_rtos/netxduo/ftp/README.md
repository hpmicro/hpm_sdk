# Netx Duo Ftp Server

## Overview

The netx duo ftp server example shows netx ftp server functions. This example enables a ftp server.

## Board Setting

No special settings are required

## Project Configuration
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number
- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Running the example

When the example runs successfully, it will print phy link status periodically until its status becomes Up. Following message is displayed in the terminal:
When accessing the ftp server, the default username is "name" and the default password is "password". After confirmation, ftp client can create, upload, delete, download files to the server. It should be noted that the capacity of the FTP server is limited, with a default of 128kB.

```console
FTP Server create success, available storage area is 1310720 bytes
Enet phy init passes !
Link Status: Down
Link Status: Up
Link Speed:  1000Mbps
Link Duplex: Full duplex
Server started!
Logged in!
```
