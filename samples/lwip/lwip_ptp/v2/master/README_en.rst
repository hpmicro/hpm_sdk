.. _lwip_ptp_v2_master:

lwip_ptp_v2_master
====================================

Overview
--------

This example shows a functionality of PTP V2 Master

- Master communicates with slave via the PTP protocol.

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect an Ethernet RGMII port or RMII port on a development board with an Ethernet RGMII port or RMII port on another development board

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

     This is an ethernet demo: PTP V2 Master
     LwIP Version: 2.1.2
     Enet phy init passed !
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     (D 1651074124.010746860) event POWER UP
     (D 1651074124.014339620) state PTP_INITIALIZING
     (D 1651074124.018623220) manufacturerIdentity: PTPd;2.0.1
     (D 1651074124.023781780) netShutdown
     (D 1651074124.027104980) netInit
     (D 1651074124.030203300) initData
     (D 1651074124.033282940) initTimer
     (D 1651074124.036426580) initClock
     (D 1651074124.039606060) state PTP_SLAVE
     (D 1651074124.043276300) initClock
     (D 1651074124.046417700) state PTP_MASTER
     (D 1651074124.050168420) SYNC INTERVAL TIMER : 1000


