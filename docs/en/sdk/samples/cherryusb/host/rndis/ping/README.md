# CherryUSB RNDIS Host - PING

## Overview

- the sample shows the USB RNDIS Host is connected to luat 4G module (air780e module) to network and ping communication


## Board Setting

- development board interconnected to air780e throught port USB0.
- development board(RNDIS Host) DEBUG port or console port connect to PC.

## Example of output log

- download the programs to the development board, and connect air780e, the board will show logs as follows
- if not, check whether the upper module is powered on

```console
Start rndis host ping task...
[I/USB] New high-speed device on Hub 1, Port 1 connected
[I/USB] New device found,idVendor:19d1,idProduct:0001,bcdDevice:0200
[I/USB] The device has 8 interfaces
[I/USB] Enumeration success, start loading class driver
[I/USB] Loading rndis class driver
[I/USB] Ep=01 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=82 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] rndis init success
[I/USB] rndis query OID_GEN_SUPPORTED_LIST success,oid num :22
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
[W/USB] Ignore rndis query iod:00010112
[W/USB] Ignore rndis query iod:00010113
[I/USB] rndis query iod:00010114 success
[W/USB] Ignore rndis query iod:00010115
[I/USB] rndis query iod:01010101 success
[I/USB] rndis query iod:01010102 success
[W/USB] Ignore rndis query iod:01010103
[I/USB] rndis query iod:01010104 success
[W/USB] Ignore rndis query iod:01010105
[I/USB] rndis set OID_GEN_CURRENT_PACKET_FILTER success
[I/USB] rndis set OID_802_3_MULTICAST_LIST success
[I/USB] Register RNDIS Class:/dev/rndis
[I/USB] Loading cdc_data class driver
[I/USB] Loading cdc_acm class driver
[I/USB] Ep=02 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=84 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Register CDC ACM Class:/dev/ttyACM0
[I/USB] Loading cdc_data class driver
[I/USB] Loading cdc_acm class driver
m[[I/USB] Ep=03 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=86 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Register CDC ACM Class:/dev/ttyACM1
[I/USB] Loading cdc_data class driver
[I/USB] Loading cdc_acm class driver
I/USB] L[I/USB] Ep=04 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=88 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Register CDC ACM Class:/dev/ttyACM2
[I/USB] Loading cdc_data class driver
oading cdc_data clinkup!!!!!!
 DHCP state       : SELECTING
 DHCP state       : REQUESTING
 DHCP state       : BOUND

 IPv4 Address     : 192.168.10.2
 IPv4 Subnet mask : 255.255.255.0
 IPv4 Gateway     : 192.168.10.1

```

- after that, the following logs will be printed, need to enter the IP addrees or domain name to be ping, and press enter key when done
- for example: www.baidu.com

```console
*********************************************************************************

input ping the IP or URL address and press the enter key to end

if want to terminate midway, please press the esc key

*********************************************************************************
Pinging www.baidu.com [120.232.145.144] ..................
from 120.232.145.144 bytes=60 icmp_seq=0 ttl=52 time=53 ms
from 120.232.145.144 bytes=60 icmp_seq=1 ttl=52 time=77 ms
from 120.232.145.144 bytes=60 icmp_seq=2 ttl=52 time=91 ms
from 120.232.145.144 bytes=60 icmp_seq=3 ttl=52 time=104 ms
from 120.232.145.144 bytes=60 icmp_seq=4 ttl=52 time=82 ms
```
