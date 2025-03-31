.. _tsn_frame_preemption_egress:

tsn_frame_preemption_egress
======================================================

Overview
--------

This example shows TSN preemption (IEEE802.1 Qbu)  input

- Observe the receiving sequence of data frames at the receiving end

  - The sender sends the preemtible traffic first, then the express traffic, and the receiver receives the express traffic first, then the preemtible traffic.

    Note: The receiving order is from the CPU point of view.
- Observe FPE and RX-Time at the receiving end

  - PFE is 1 and RX-Time is 0 for the preemtible traffic,  while FPE is 0 and RX-Time is non-zero for the express traffic.

- Observe whether the value of the FPE Assembly Ok Counter is incremented synchronously with the sender

  - Synchronous increment indicates successful receipt of preemptible traffic.

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

     This is a TSW demo: Frame Preemption Egress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     FPE: 0, RX-Time: 3.399945890
     Rx Frame Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     FPE Assembly Ok Counter: 2
     ==============================================================================================================================================================================================================
     FPE: 1, RX-Time: 0.000000000
     Rx Frame Length: 512
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     FPE Assembly Ok Counter: 2
     ==============================================================================================================================================================================================================



