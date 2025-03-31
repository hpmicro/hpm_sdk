.. _netx_duo_sntp_client:

Netx Duo Sntp Client
========================================

Overview
--------

The netx duo sntp client example shows netx sntp client functions. This example enables a sntp client. The client will try to get time information from server 192.168.1.2.

Board Setting
-------------

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

Project Configuration
---------------------

- Ethernet Port Settings: Refer to :ref:`Ethernet common project settings <ethernet_port_configurations_en>`

- By `set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)` in CMakeLists.txt, the DHCP will be enabled

Running the example
-------------------

When the example runs successfully, it will print phy link status periodically until its status becomes Up. Following message is displayed in the terminal:
The program will request time data in a fixed point manner, and when the time is obtained, it will be printed to the terminal. Note: The routine requires that the root partition of the sntp server cannot be greater than 5 seconds.


.. code-block:: console

   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   year [2023] :month [5]: day [5] :hour  [11]: min  [21]: second  [51]
   SNTP updated
   Time: 3892245711.435 sec.

