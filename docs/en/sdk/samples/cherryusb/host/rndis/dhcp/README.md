# USB RNDIS HOST - DHCP

## Overview

- This example project shows USB RNDIS HOST realize DHCP Client.
- Using two development boards interconnect. A development board program 'samples/cheryusb/device/rndis/udp_echo' as the RNDIS device, and B development board program this example as the RNDIS host. The RNDIS host will obtain and set its own MAC address from the RNDIS device, as well as obtain its own IP address and gateway information through the dhcp protocol.

## Board Setting

- A development board interconnected to B development board throught port USB0.
- B development board(RNDIS HOST) DEBUG port or console port connect to PC.

## Example of output log

- Download the two programs to the A development board and B development board respectively, and connect them properly. B development board(RNDIS HOST) console will show this log:

```console

Start cherryusb rndis host task...
[I/USB] New high-speed device on Hub 1, Port 1 connected
[I/USB] New device found,idVendor:34b7,idProduct:ffff,bcdDevice:0100
[I/USB] The device has 2 interfaces
[I/USB] Enumeration success, start loading class driver
[I/USB] Loading rndis class driver
[I/USB] Ep=02 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=81 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] rndis init success
[I/USB] rndis query OID_GEN_SUPPORTED_LIST success,oid num :29
[W/USB] Ignore rndis query iod:00010101
[W/USB] Ignore rndis query iod:00010102
[W/USB] Ignore rndis query iod:00010103
[W/USB] Ignore rndis query iod:00010104
[I/USB] rndis query iod:00010106 success
[I/USB] rndis query iod:00010107 success
[W/USB] Ignore rndis query iod:0001010a
[W/USB] Ignore rndis query iod:0001010b
[W/USB] Ignore rndis query iod:0001010c
[W/USB] Ignore rndis query iod:0001010d
[W/USB] Ignore rndis query iod:00010116
[W/USB] Ignore rndis query iod:0001010e
[W/USB] Ignore rndis query iod:00010111
[I/USB] rndis query iod:00010114 success
[I/USB] rndis query iod:00010202 success
[W/USB] Ignore rndis query iod:00020101
[W/USB] Ignore rndis query iod:00020102
[W/USB] Ignore rndis query iod:00020103
[W/USB] Ignore rndis query iod:00020104
[W/USB] Ignore rndis query iod:00020105
[W/USB] Ignore rndis query iod:0001021b
[I/USB] rndis query iod:01010101 success
[I/USB] rndis query iod:01010102 success
[W/USB] Ignore rndis query iod:01010103
[I/USB] rndis query iod:01010104 success
[W/USB] Ignore rndis query iod:01020101
[W/USB] Ignore rndis query iod:00000001
[W/USB] Ignore rndis query iod:00000000
[W/USB] Ignore rndis query iod:000001a1
[I/USB] rndis set OID_GEN_CURRENT_PACKET_FILTER success
[I/USB] rndis set OID_802_3_MULTICAST_LIST success
[I/USB] Register RNDIS Class:/dev/rndis

************************************************
 DHCP example
************************************************
[I/USB] Loading cdc_data class driver
 DHCP state       : SELECTING
 DHCP state       : REQUESTING
 DHCP state       : BOUND

 IPv4 Address     : 192.168.7.2
 IPv4 Subnet mask : 255.255.255.0
 IPv4 Gateway     : 192.168.7.1

rndis dev keepalive success!
rndis dev keepalive success!
rndis dev keepalive success!


```
