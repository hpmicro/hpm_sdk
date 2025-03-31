.. _netx_duo_ftp_server:

Netx Duo Ftp Server
======================================

Overview
--------

The netx duo ftp server example shows netx ftp server functions. This example enables a ftp server.

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
When accessing the ftp server, the default username is "name" and the default password is "password". After confirmation, ftp client can create, upload, delete, download files to the server. It should be noted that the capacity of the FTP server is limited, with a default of 128kB.


.. code-block:: console

   FTP Server create success, available storage area is 1310720 bytes
   Enet phy init passed !
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   Server started!
   Logged in!

