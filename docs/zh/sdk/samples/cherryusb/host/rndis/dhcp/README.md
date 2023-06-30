# USB RNDIS HOST - DHCP

## 概述

- 本示例工程展示USB RNDIS HOST实现DHCP Client。
- 使用两块开发板对接，A开发板烧写samples/cherryusb/device/rndis/udp_echo程序作为rndis设备，B开发板烧写本示例程序作为rndis主机。rndis主机将会从rndis设备中获取并设置自己的MAC地址，也会通过dhcp协议获取自己的ip地址及网关信息等。

## 硬件设置

- 两块开发板的USB0通过USB线连接。
- B开发板(RNDIS HOST)的DEBUG口或UART Console口接于PC。

## 运行现象

- 将两个程序分别下载至A开发板和B开发中，并接好连线，B开发板(RNDIS HOST)串口控制台输出如下：
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
