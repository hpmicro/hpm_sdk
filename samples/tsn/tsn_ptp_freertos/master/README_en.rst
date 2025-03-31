.. _tsn_ptp_freertos_master:

tsn_ptp_freertos_master
==============================================

Overview
--------

This example shows a functionality of TSN PTP Master

- Master communicates with slave via the gPTP protocol.

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect an TSN RGMII port on a development board with an TSN RGMII port on another development board

Project Configurations
----------------------

- None

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


.. code-block:: console

     This is a TSW demo: PTP Master
     TSW phy init passed !
     starting task 'syn eth0' ... done
     PTP4TSYNC | add domain 0
     PTP4TSYNC | set ptp traffic priority=0 (Background) for domain 0
     PTP4TSYNC | add eth0 to domain 0
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     domain   0: domaintime=1726133403.019751880 or Thu Sep 12 09:30:03 2024
     domain   0: domaintime=1726133406.025912960 or Thu Sep 12 09:30:06 2024
     domain   0: domaintime=1726133409.031911990 or Thu Sep 12 09:30:09 2024


