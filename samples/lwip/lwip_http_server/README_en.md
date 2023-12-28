# lwip_http_server

## Overview

This example shows how to use lwip http to setup a http server.

Note:

* An exception may occur if you repeatedly refresh the web page.

## Board Settings

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

## Project Configurations

- Ethernet Port Settings: Refer to [Ethernet common project settings](../doc/Ethernet_Common_Project_Settings_en.md)
- Ethernet DHCP Configurations
    - the configurations in the `CMakeLists.txt` as follows:
      - sdk_compile_definitions(-DLWIP_DHCP=0): Disable DHCP feature
      - sdk_compile_definitions(-DLWIP_DHCP=1): Enable DHCP feature


## Run Example

- Compiling and Downloading
- Running log is shown in the serial terminal as follows:

```console
This is an ethernet demo: HTTP Server (Polling Usage)
LwIP Version: 2.1.2
Reference Clock: Internal Clock
Enet phy init passed !
Static IP: 192.168.100.10
Netmask  : 255.255.255.0
Gateway  : 192.168.100.1
Link Status: Down
Link Status: Down
Link Status: Up
Link Speed:  100Mbps
Link Duplex: Full duplex
```

Open browser on PC, type http://192.168.100.10 and press enter, the web page will appear：

Note: This demo can run successfully via 360 browser or IE browser.

![](doc/lwip_httpsrv.png)
