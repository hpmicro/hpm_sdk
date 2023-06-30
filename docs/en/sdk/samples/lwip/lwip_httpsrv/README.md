# Http Server

## Overview

This example shows how to use lwip http to setup a http server.

Note:

* An exception may occur if you repeatedly refresh the web page.

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

## Project Configuration

- In the file `CMakeLists.txt` ,  set a matched Ethernet PHY type according to the development board schematic，

  e.g. "set(CONFIG_ENET_PHY_DP83848 1)"

## Run Example

- Compiling and Downloading
- Running log is shown in the serial terminal as follows:

```console
This is an ethernet demo: HTTP Server (Polling Usage)
LwIP Version: 2.1.2
Reference Clock: Internal Clock
Enet phy init passes !
Static IP: 10.10.10.10
NETMASK  : 255.255.255.0
Gateway  : 10.10.10.1
Link Status: Down
Link Status: Down
Link Status: Up
Link Speed:  100Mbps
Link Duplex: Full duplex
```

Open browser on PC, type http://10.10.10.10 and press enter, the web page will appear：

Note: This demo can run successfully via 360 browser or IE browser.

![](../../../../../assets/sdk/samples/lwip_httpsrv.png)
