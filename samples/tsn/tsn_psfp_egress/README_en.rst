.. _tsn_psfp_egress:

tsn_psfp_egress
==============================

Overview
--------

This example shows TSN per-stream filtering and policing (IEEE802.1Qci) input

- Observe whether the length of the incoming traffic is within the limit

  - Enable  size filtering:  Allows traffic through a limited length.

- Observe whether the incoming traffic is controlled by gating

  - Enable static gating filtering:  The static gating configuration determines whether to allow traffic through.

  - Enable dynamic gating filtering: The dynamic gating status determines whether to allow traffic through.

- Observe whether the incoming traffic is within the bandwidth limit

  - Enable flow metering: Allows traffic through within  the specified (or excess) bandwidth.

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

     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     RX-Time: 3.174851740
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     RX-Time: 3.696005720
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     RX-Time: 4.217157370
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     RX-Time: 4.738308540
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     RX-Time: 5.259460210
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     RX-Time: 5.780611410
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     RX-Time: 6.301762940
     RX Length: 60
     ff ff ff ff ff ff 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00


