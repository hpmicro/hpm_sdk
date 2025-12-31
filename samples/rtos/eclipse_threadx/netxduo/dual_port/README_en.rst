.. _netx_duo_dual_port:

Netx Duo Dual Port
==========================

Overview
----------

This example demonstrates the dual network interface functionality of NetX. It enables responses to ping commands, starts a TCP client on network interface 0, and starts a TCP server on network interface 1.

Hardware Setup
-----------------

Connect the development board and PC/router/switch with an Ethernet cable

Project Configuration
------------------------

- Ethernet port configuration: Refer to `Ethernet Common Project Settings <../../../../lwip/doc/Ethernet_Common_Project_Settings_zh.rst>`_

- This project only supports static IP address configuration. If DHCP is required, please refer to other netxduo examples to add DHCP-related code.

Running Behavior
------------------

When the project runs correctly, the link status will be periodically printed to the terminal until the link status is Up, and the number of received ICMP packets will also be periodically printed to the terminal.
For network interface 0, the program will attempt to connect to **SERVER_IPV4_ADDRESS : ECHO_SERVER_PORT** (192.168.100.3:7777). Once connected to the server, the program will send the message **"ABCDEFGHIJKLMNOPQRSTUVWXYZ "** to the server, then wait for the server to return data until data is actually received or the server closes the connection.
For network interface 1, the program will listen on port **ECHO_SERVER_PORT** (7777). Once a client connects to the server, the program will wait for the client to send data until data is received, 10 seconds pass without receiving data, or the client closes the connection. If data is received from the client, the program will return the same data to the client and then close the connection.
The following messages will be printed to the terminal.

.. code-block:: console

Enet1 Reference Clock: Internal Clock
NetXDuo is running
ETH0 IP address: 192.168.100.223
ETH0 Mask: 255.255.255.0
ETH1 IP address: 192.168.99.223
ETH1 Mask: 255.255.255.0
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
Enet RGMII phy init passed !
Enet RMII phy init passed !
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
ETH0 Link Status: Down
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
ETH0 received 0 ICMP packets, ETH1 received 0 ICMP packets
ETH0 Link Status: Up
ETH0 Link Speed:  1000Mbps
ETH0 Link Duplex: Full duplex
ETH0 received 41 ICMP packets, ETH1 received 0 ICMP packets
ETH0 received 61 ICMP packets, ETH1 received 0 ICMP packets
ETH0 received 81 ICMP packets, ETH1 received 0 ICMP packets

