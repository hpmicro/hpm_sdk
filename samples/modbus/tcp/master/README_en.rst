.. _modbus_tcp_master:

modbus tcp master
==================================

Overview
--------

- This project demonstrates the modbus tcp master function

- In this example project, one board serves as tcp master and the other board serves as tcp slave. The master sends a read holding register and waits for the slave to respond to the request data.

Note
----

- When using enet for modbus communication and communicating with an tcp slave, you must reset the slave first and then the host.

Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect the RGMII port or a RMII port of the two boards.

Running the example
-------------------

- Run slave first, then master

- When the project runs correctly, the serial terminal will output the following information:


.. code-block:: console

   This is an ethernet demo: modbus tcp (Polling Usage)
   LwIP Version: 2.1.2
   Enet phy init passed !
   Static IP: 192.168.100.11
   Netmask  : 255.255.255.0
   Gateway  : 192.168.100.1
   modbus tcp master example
   Link Status: Down
   client connected server
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

