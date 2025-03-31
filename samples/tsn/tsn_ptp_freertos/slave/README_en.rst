.. _tsn_ptp_freertos_slave:

tsn_ptp_freertos_slave
============================================

Overview
--------

This example shows a functionality of TSN PTP slave

- Master communicates with slave via the gPTP protocol.

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a TSN RGMII port on a development board with a TSN RGMII port on another development board

Project Configurations
----------------------

- None

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


.. code-block:: console

     This is a TSW demo: PTP Slave
     TSW phy init passed !
     starting task 'syn eth0' ... done
     PTP4TSYNC | add domain 0
     PTP4TSYNC | set ptp traffic priority=0 (Background) for domain 0
     PTP4TSYNC | add eth0 to domain 0
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     domain   0: domaintime=1725874203.019739580 or Mon Sep 09 09:30:03 2024
     PTP4TSYNC | assign sync source to domain 0
     domain   0: domaintime=1726133607.430393244 or Thu Sep 12 09:33:27 2024
     domain   0: domaintime=1726133610.436395863 or Thu Sep 12 09:33:30 2024


