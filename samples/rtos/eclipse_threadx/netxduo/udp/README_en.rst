.. _netx_duo_udp:

Netx Duo Udp
========================

Overview
--------

The netx duo udp example shows netx udp send and receive functions.

Board Setting
-------------

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

Project Configuration
---------------------

- Ethernet Port Settings: Refer to :ref:`Ethernet common project settings <ethernet_port_configurations_en>`

- By `set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)` in CMakeLists.txt, the DHCP will be enabled

- When enabling DHCP, please note that the IP address represented by macro ECHO_SERVER_ADDRESS should be accessible through the gateway.

Running the example
-------------------

When the example runs successfully, it will send udp message **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "**  to **ECHO_SERVER_ADDRESS : ECHO_SERVER_PORT** (192.168.1.2:7777) through port **ECHO_SEND_PORT** (7777) and then wait for receive udp message with a timeout. It will print phy link status periodically until its status becomes **Up** . If received udp message, it will also print it to the console. Following message is displayed in the terminal:

.. code-block:: console

   NetXDuo is running
   IP address: 192.168.1.223
   Mask: 255.255.255.0
   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   No response from server 1
   No response from server 2
   No response from server 3
   No response from server 4
   Receive response: abcdefg 5
   Receive response: abcdefg 6
   Receive response: abcdefg 7
   Receive response: abcdefg 8
   Receive response: abcdefg 9
   Receive response: abcdefg 10
   Receive response: abcdefg 11

