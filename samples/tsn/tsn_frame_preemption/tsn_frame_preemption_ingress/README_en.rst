.. _tsn_frame_preemption_ingress:

tsn_frame_preemption_ingress
========================================================

Overview
--------

This example shows TSN frame preemption(IEEE802.1 Qbu) output

- Observe the value of  fragment tx counter from MMS

  - Increment indicates that the preemptible traffic is successfully sent.

- On the receiving end, run the sample tsn_frame_preemption_egress on another development board and observe the order of the receiving frame and the value of  assembly ok counter.

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect RGMII ports on the two development board with an Ethernet cable

Project Configurations
----------------------

- None

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


.. code-block:: console

     This is a TSW demo: Frame Preemption Ingress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     FPE MMS Fragment Tx Counter: 1
     ==================================================================
     FPE MMS Fragment Tx Counter: 2
     ==================================================================
     FPE MMS Fragment Tx Counter: 3
     ==================================================================
     FPE MMS Fragment Tx Counter: 4
     ==================================================================
     FPE MMS Fragment Tx Counter: 5
     ==================================================================
     FPE MMS Fragment Tx Counter: 6
     ==================================================================
     FPE MMS Fragment Tx Counter: 7
     ==================================================================
     FPE MMS Fragment Tx Counter: 8
     ==================================================================


