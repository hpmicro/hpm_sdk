# RNDIS Host - DHCP Client

## 概述

- 本示例工程展示USB RNDIS或者ECM Host 实现DHCP，自动分配IP 连接合宙4G模块-air780e模块或者EC20模块实现DHCP自动分配IP。

## 硬件设置

- 开发板的USB0通过USB线连接合宙4G air780e模块或者EC20模块。
- 开发板(RNDIS/ECM Host)的DEBUG口或UART Console口接于PC。

## 运行现象

- 将程序分别下载至开发板中，并接好连线，开发板(RNDIS Host)串口控制台输出如下，表示air780e模块已经初始化成功，若没有则检查上模块是否上电
```console

Start cherryusb host usbnet task...
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

```

- 当连接的是EC20模块，则打印以下信息，表示EC20初始化成功并分配好IP

```console

[I/USB] New high-speed device on Hub 1, Port 1 connected
[I/USB] New device found,idVendor:2c7c,idProduct:0125,bcdDevice:0318
[I/USB] The device has 6 interfaces
[I/USB] Enumeration success, start loading class driver
[E/USB] do not support Class:0xff,Subclass:0xff,Protocl:0xff
[E/USB] do not support Class:0xff,Subclass:0x00,Protocl:0x00
[E/USB] do not support Class:0xff,Subclass:0x00,Protocl:0x00
[E/USB] do not support Class:0xff,Subclass:0x00,Protocl:0x00
[I/USB] Loading cdc_ecm class driver
[I/USB] CDC ECM mac address 9e: 28: 59: e0: 00: 00
[I/USB] CDC ECM Max Segment Size:1280
[I/USB] Ep=89 Attr=03 Mps=16 Interval=09 Mult=00
[I/USB] Ep=88 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=05 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Select cdc ecm altsetting: 1
[I/USB] Set CDC ECM packet filter:000c
[I/USB] Register CDC ECM Class:/dev/cdc_ether
[E/USB] do not support Class:0x0a,Subclass:0x00,Protocl:0x00
 DHCP state       : SELECTING
 DHCP state       : BOUND

 IPv4 Address     : 192.168.225.27
 IPv4 Subnet mask : 255.255.255.0
 IPv4 Gateway     : 192.168.225.1

```

注意: 切换rndis模块或者ecm模块的时候，也需要复位下开发板。
