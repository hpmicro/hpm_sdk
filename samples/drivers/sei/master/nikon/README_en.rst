.. _sei_master_connect_nikon_encoder:

SEI Master: Connect NIKON Encoder
==================================================================

Overview
----------

This demo shows SEI interface read NIKON encoder (Product: MAR-MC42AHN00) position data and automatically calculate sampling/update delay time, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Slave sample path is "samples/drivers/sei/slave/nikon", master is this example.

- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.

- Connect master's GND to slave's GND.

Running the example
-------------------

- When the example runs successfully, the console shows the following log. ST's value increasing by 1 each sample.


.. code-block:: console

   ----------------------------------------------------------------------
   $$\   $$\ $$$$$$$\  $$\      $$\ $$\
   $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
   $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
   $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
   $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
   $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
   $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
   \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
   ----------------------------------------------------------------------
   SEI master nikon sample
   Started sei engine!
   ST:0xfffa5, MT:0x8888, CRC:0x63, TimeDelta:307*0.1us
   ST:0xfffa6, MT:0x8888, CRC:0x5e, TimeDelta:307*0.1us
   ST:0xfffa7, MT:0x8888, CRC:0x9a, TimeDelta:307*0.1us
   ST:0xfffa8, MT:0x8888, CRC:0x53, TimeDelta:307*0.1us
   ST:0xfffa9, MT:0x8888, CRC:0x97, TimeDelta:307*0.1us
   ST:0xfffaa, MT:0x8888, CRC:0xaa, TimeDelta:307*0.1us
   ST:0xfffab, MT:0x8888, CRC:0x6e, TimeDelta:307*0.1us
   ST:0xfffac, MT:0x8888, CRC:0xd0, TimeDelta:307*0.1us
   ST:0xfffad, MT:0x8888, CRC:0x14, TimeDelta:307*0.1us
   ST:0xfffae, MT:0x8888, CRC:0x29, TimeDelta:307*0.1us
   ST:0xfffaf, MT:0x8888, CRC:0xed, TimeDelta:307*0.1us
   ST:0xfffb0, MT:0x8888, CRC:0xca, TimeDelta:307*0.1us
   ST:0xfffb1, MT:0x8888, CRC:0xe, TimeDelta:307*0.1us
   ST:0xfffb2, MT:0x8888, CRC:0x33, TimeDelta:307*0.1us
   ST:0xfffb3, MT:0x8888, CRC:0xf7, TimeDelta:307*0.1us
   ST:0xfffb4, MT:0x8888, CRC:0x49, TimeDelta:307*0.1us

