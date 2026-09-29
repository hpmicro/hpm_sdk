.. _sei_slave_simulate_nikon_encoder:

SEI Slave: Simulate NIKON Encoder
==================================================================

Overview
----------

This demo shows SEI interface simulate NIKON encoder (Product: MAR-MC42AHN00) and generate position data, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Master sample path is "samples/drivers/sei/master/nikon", slave is this example.

- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.

- Connect master's GND to slave's GND.

Running the example
-------------------

- When the example runs successfully, the console shows the following log. ST's value increasing by 1 each sample, sample interval is 200ms.


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
   SEI slave nikon sample
   Started sei engine!
   EAX:0x4, CC:0, ST:0xfffa5, MT:0x8888, CRC:0x63, sample_interval:1147104 us
   EAX:0x4, CC:0, ST:0xfffa6, MT:0x8888, CRC:0x5e, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffa7, MT:0x8888, CRC:0x9a, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffa8, MT:0x8888, CRC:0x53, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffa9, MT:0x8888, CRC:0x97, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffaa, MT:0x8888, CRC:0xaa, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffab, MT:0x8888, CRC:0x6e, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffac, MT:0x8888, CRC:0xd0, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffad, MT:0x8888, CRC:0x14, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffae, MT:0x8888, CRC:0x29, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffaf, MT:0x8888, CRC:0xed, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb0, MT:0x8888, CRC:0xca, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb1, MT:0x8888, CRC:0xe, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb2, MT:0x8888, CRC:0x33, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb3, MT:0x8888, CRC:0xf7, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb4, MT:0x8888, CRC:0x49, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb5, MT:0x8888, CRC:0x8d, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb6, MT:0x8888, CRC:0xb0, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb7, MT:0x8888, CRC:0x74, sample_interval:19999 us

