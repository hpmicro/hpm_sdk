.. _tsn_switch:

tsn_switch
==========

Overview
--------

This example shows TSW working as a basic network switch.

Board Setting
-------------

Select any two ports on the switch and then connect to two PCs respectively (or network devices)

Running the example
-------------------

The two PCs (or network devices) communicate with each other.

After startup, the serial console prints configuration for all three ports and reports link changes per port, for example:

.. code-block:: console

   Port1: MII 100Mbps, Switch MAC 98:2c:bc:b1:9f:17, Lookup dest MAC 38:14:28:14:90:78
   Port2: MII 100Mbps, Switch MAC 98:2c:bc:b1:9f:18, Lookup dest MAC 38:14:28:14:90:79
   Port3: RGMII 1000Mbps, Switch MAC 98:2c:bc:b1:9f:19, Lookup dest MAC 38:14:28:14:90:7a

   ================ TSW PHY on Port1 ================
   Link Status: Up
   Link Speed:  100Mbps
   Link Duplex: Full duplex

   ================ TSW PHY on Port2 ================
   Link Status: Up
   Link Speed:  100Mbps
   Link Duplex: Full duplex

   ================ TSW PHY on Port3 ================
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex

Note
----

1. The IP addresses of the two PCs (or network devices) must be on the same network segment.
2. Please refer to errata for detailed information of usage precautions.