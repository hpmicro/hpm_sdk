# Netx Duo Tcp server

## Overview

The netx duo tcp server example shows netx tcp server functions.

## Board Setting

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

## Project Configuration
- Ethernet Port Settings: Refer to [Ethernet common project settings](../../../../lwip/doc/Ethernet_Common_Project_Settings_en.md)
- By Setting `CONFIG_NETX_ENABLE_DHCP` to zero or comment the definition code in CMakeLists.txt, the DHCP will be disabled
- When DHCP is disabled, comment or delete `set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)` can decrease project source files number


## Running the example

When the example runs successfully, it will listen to port **ECHO_SERVER_PORT**(7777). Once a client connected, it will wait for receiving message from the client with a timeout 10 seconds. It will send data received from the client back to the client and then close the connection. It will print phy link status periodically until its status becomes **Up**. Following message is displayed in the terminal:
```console
Reference Clock: Internal Clock
Enet phy init passed !
Link Status: Down
NetXDuo is running

IP address: 192.168.1.223

Mask: 255.255.255.0

Link Status: Up
Link Speed:  100Mbps
Link Duplex: Full duplex
A client is connected to the server.
Receive data from client.
Send data back to client success.
Close the connection
A client is connected to the server.
Receive data from client.
Send data back to client success.
Close the connection
A client is connected to the server.
Failed to receive from the socket, reason 1.
Close the connection
```
