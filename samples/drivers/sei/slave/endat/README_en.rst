.. _sei_slave_simulate_endat_encoder:

SEI Slave: Simulate ENDAT Encoder
==================================================================

Overview
----------

This demo shows SEI interface simulate ENDAT encoder and generate position data, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Master sample path is "samples/drivers/sei/master/endat", slave is this example.

- Master's SEI_CLK pins should bridging master side，and slave's SEI_CLK pins should bridging slave side.

- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.

- Connect master's SEI pins CLKO_P/CLKO_N to slave's SEI pins CLKI_P/CLKI_N.

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
   SEI slave EnDat sample
   Started sei engine!
   CMD:0x7, E:0x0, ST:0xa5a5, CRC:0x5, sample_tm1:20525173, sample_tm2:0, sample_interval:128282 us
   CMD:0x7, E:0x0, ST:0xa5a6, CRC:0xf, sample_tm1:52524852, sample_tm2:20525173, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5a7, CRC:0x1a, sample_tm1:84524531, sample_tm2:52524852, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5a8, CRC:0x7, sample_tm1:116524210, sample_tm2:84524531, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5a9, CRC:0x12, sample_tm1:148523889, sample_tm2:116524210, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5aa, CRC:0x18, sample_tm1:180523568, sample_tm2:148523889, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5ab, CRC:0xd, sample_tm1:212523247, sample_tm2:180523568, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5ac, CRC:0x1d, sample_tm1:244522927, sample_tm2:212523247, sample_interval:199998 us

