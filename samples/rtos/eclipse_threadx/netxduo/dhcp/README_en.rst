.. _netx_duo_dhcp:

Netx Duo Dhcp
==========================

Overview
--------

The netx duo dhcp example shows netx dhcp functions.

Board Setting
-------------

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

Project Configuration
---------------------

- Ethernet Port Settings: Refer to :ref:`Ethernet common project settings <ethernet_port_configurations_en>`

Running the example
-------------------

When the example runs successfully, it will print phy link status periodically until its status becomes Up, and it will print DHCP progress. Following message is displayed in the terminal:

.. code-block:: console

   NetXDuo is running␍␍␊
   DHCP In Progress...␍␍␊
   Enet phy init passed !␍␊
   Link Status: Down␍␊
   Link Status: Up␍␊
   Link Speed:  100Mbps␍␊
   Link Duplex: Full duplex␍␊
   IP address: 192.168.50.76␍␍␊
   Mask: 255.255.255.0␍␍

