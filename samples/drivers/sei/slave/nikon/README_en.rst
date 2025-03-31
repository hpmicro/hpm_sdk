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
   EAX:0x4, CC:0x0, ST:0xfffa5, MT:0x8888, CRC:0xa9, sample_tm1:16614570, sample_tm2:0, sample_interval:103841 us
   EAX:0x4, CC:0x0, ST:0xfffa6, MT:0x8888, CRC:0x7a, sample_tm1:48614245, sample_tm2:16614570, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffa7, MT:0x8888, CRC:0x59, sample_tm1:80613920, sample_tm2:48614245, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffa8, MT:0x8888, CRC:0xca, sample_tm1:112613594, sample_tm2:80613920, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffa9, MT:0x8888, CRC:0xe9, sample_tm1:144613269, sample_tm2:112613594, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffaa, MT:0x8888, CRC:0x55, sample_tm1:176612944, sample_tm2:144613269, sample_interval:199997 us

