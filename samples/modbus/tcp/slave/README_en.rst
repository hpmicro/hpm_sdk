.. _modbus_tcp_slave:

modbus tcp slave
================================

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

.. code-block:: console

    This is an ethernet demo: modbus tcp (Polling Usage)
    LwIP Version: 2.1.2
    Enet phy init passed !
    Static IP: 192.168.100.10
    Netmask  : 255.255.255.0
    Gateway  : 192.168.100.1
    modbus tcp slave example
    Link Status: Down
    server litened
    parse ok, recv master msg len:12
    Link Status: Up
    Link Speed:  1000Mbps
    Link Duplex: Full duplex
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12
    parse ok, recv master msg len:12

