.. _tsn_stmid_frer_egress:

tsn_stmid_frer_egress
==========================================

Overview
--------

This example shows TSN frame identification and redundancy elimination (IEEE802.1 CB)

- Observe information of frames

  - The observed frames are the frame after redundancy elimination algorithms.

  - The sequence number of frame is the same as the original frame

  - "Presented Frames" indicates the number of frames that have been recovered.

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

     This is a TSW demo: CB Stmid Frer Egress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     Valid: 1, SID: 1, SeqNo: 0
     RX-Time: 20.295925680
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 1

     Valid: 1, SID: 1, SeqNo: 1
     RX-Time: 20.799703960
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 2

     Valid: 1, SID: 1, SeqNo: 2
     RX-Time: 21.303476990
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 3

     Valid: 1, SID: 1, SeqNo: 3
     RX-Time: 21.807250010
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 4

     Valid: 1, SID: 1, SeqNo: 4
     RX-Time: 22.311024700
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 5

     Valid: 1, SID: 1, SeqNo: 5
     RX-Time: 22.814797840
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 6



