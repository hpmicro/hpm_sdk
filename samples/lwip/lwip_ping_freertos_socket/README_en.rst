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

  - the configurations in the `CMakeLists.txt` as follows:

    - sdk_compile_definitions(-DLWIP_DHCP=0): Disable DHCP feature

    - sdk_compile_definitions(-DLWIP_DHCP=1): Enable DHCP feature

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


  .. code-block:: console

     This is an ethernet demo: Ping With Socket API On FreeRTOS
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     DHCP State: SELECTING
     DHCP State: CHECKING
     DHCP State: BOUND
     IPv4 Address: 192.168.11.25
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.11.254
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


