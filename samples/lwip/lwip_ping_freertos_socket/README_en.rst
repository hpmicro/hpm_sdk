.. _lwip_ping_freertos_socket:

lwip_ping_freertos_socket
==================================================

Overview
--------

This example shows the usage of ping  on FreeRTOS.

- MCU pings the specified network device( Enter IP or URL)

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

Project Configurations
----------------------

- Ethernet Port Settings: Refer to :ref:`Ethernet common project settings <ethernet_port_configurations_en>`

- Ethernet DHCP Configurations

  - In `CMakeLists.txt`, if CMake variable `LWIP_DHCP` is **not** defined, it defaults to **0** (static IP). The build passes `-DLWIP_DHCP=${LWIP_DHCP}`.

    - **Default (static IP)**: configure `IP0_CONFIG` / `NETMASK0_CONFIG` / `GW0_CONFIG` in `netinfo.h` (or override via compile definitions). DNS defaults to 114.114.114.114 when not using DHCP.

    - **Enable DHCP**: pass `-DLWIP_DHCP=1` when configuring CMake (e.g. ``cmake -DLWIP_DHCP=1 ...``), or set `LWIP_DHCP` to 1 in `CMakeLists.txt` before the `if(NOT DEFINED LWIP_DHCP)` block.

  - DNS (static IP mode, applies to default build):

    - Default DNS is 114.114.114.114. If the MCU can resolve domestic names but not international ones while the PC works, DNS may differ. Before ``#if !LWIP_DHCP`` in `lwipopts_app.h`, add ``#define DNS_SERVER_CONFIG 192.168.100.1`` (or your gateway IP) to align with the PC.

Run Example
-----------

- Compiling and Downloading

- Running log with the **default** build (static IP, `LWIP_DHCP=0`) is shown in the serial terminal as follows. If you build with DHCP enabled (`-DLWIP_DHCP=1`), lines such as ``DHCP State: ...`` appear before the IPv4 lines; addresses follow your network.


  .. code-block:: console

     This is an ethernet demo: Ping With Socket API On FreeRTOS
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     Please enter a target name(IP or URL):



- Enter an IP or URL, and then press "Enter" to confirm


  .. code-block:: console

     www.hpmicro.com



- View log:


  .. code-block:: console

     Pinging www.hpmicro.com [47.108.71.172] ..................
     from 47.108.71.172 bytes=60 icmp_seq=0 ttl=48 time=33 ms
     from 47.108.71.172 bytes=60 icmp_seq=1 ttl=48 time=33 ms
     from 47.108.71.172 bytes=60 icmp_seq=2 ttl=48 time=33 ms
     from 47.108.71.172 bytes=60 icmp_seq=3 ttl=48 time=33 ms


