.. _netx_duo_https_server:

Netx Duo Https Server
==========================================

Overview
--------

The netx duo https server example shows netx https server functions. This example enables a https server.

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
When accessing the HTTPs server, a window will pop up asking for a username and password. The default username is "name" and the default password is "password". After confirmation, the page will be displayed.


.. code-block:: text

   Test response from server



.. code-block:: console

   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex

