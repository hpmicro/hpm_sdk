.. _netx_duo_ping:

Netx Duo Ping
==========================

Overview
--------

The netx duo ping example shows netx arp and icmp functions. This example enables ping response function.

Board Setting
-------------

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

Project Configuration
---------------------

- Ethernet Port Settings: Refer to :ref:`Ethernet common project settings <ethernet_port_configurations_en>`

- By `set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)` in CMakeLists.txt, the DHCP will be enabled

Running the example
-------------------

When the example runs successfully, it will print phy link status periodically until its status becomes Up, and it will print ICMP packets received count periodically. Following message is displayed in the terminal:

.. code-block:: console

   NetXDuo is running
   IP address: 192.168.1.223
   Mask: 255.255.255.0
   received 0 ICMP packets
   Enet phy init passed !
   received 0 ICMP packets
   Link Status: Down
   received 0 ICMP packets
   received 0 ICMP packets
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   received 0 ICMP packets
   received 0 ICMP packets
   received 0 ICMP packets
   received 0 ICMP packets
   received 0 ICMP packets
   received 0 ICMP packets
   received 1 ICMP packets
   received 2 ICMP packets
   received 3 ICMP packets
   received 4 ICMP packets

