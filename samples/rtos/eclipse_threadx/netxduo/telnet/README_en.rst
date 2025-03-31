.. _netx_duo_telnet:

Netx Duo Telnet
==============================

Overview
--------

The netx duo telnet example shows netx telnet server functions. This example enables a telnet server.

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

.. code-block:: console

   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex


When telnet client connect to the host, type a character in the client, the character will echo in the telnet terminal, specially enter a **q**  will cause server close the connection. When enter a **Enter**  key, it will turn to a new line.
