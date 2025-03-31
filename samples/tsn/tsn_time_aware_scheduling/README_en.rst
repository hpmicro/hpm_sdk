.. _tsn_time_aware_scheduling:

tsn_time_aware_scheduling
==================================================

Overview
--------

This example shows TSN TAS(IEEE802.1 Qbv) traffic shaper output

- Watch traffic shaper output through an Ethernet packet capture tool on PC

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect an Ethernet port on PC to a RGMII port on the development board with an Ethernet cable

Project Configurations
----------------------

- None

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


.. code-block:: console

     This is a TSW demo: Time Aware Scheduing
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex


- Run an Ethernet packet capture tool

  - set a filter rule: eth.src == 98:2c:bc:b1:9f:17 and arp

    **Note: There is a necessary adjustment for "eth.src" according to the actual TSN mac**
  - Watch the traffic sharper output: traffic interval/traffic cycle

    .. image:: doc/tsn_time_aware_scheduling.png
       :alt: img
