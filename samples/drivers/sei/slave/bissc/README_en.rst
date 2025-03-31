.. _sei_slave_simulate_bissc_encoder:

SEI Slave: Simulate BISSC Encoder
==================================================================

Overview
----------

This demo shows SEI interface simulate BISSC encoder and generate position data, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Master sample path is "samples/drivers/sei/master/bissc", slave is this example.

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
   SEI slave BiSS_C sample
   Started sei engine!
   MT:0xa5a, ST:0x5a5, EW:0x3, CRC:0x7991, sample_tm1:217221230, sample_tm2:0, sample_interval:1206784 us
   MT:0xa5a, ST:0x5a6, EW:0x3, CRC:0x6, sample_tm1:253221299, sample_tm2:217221230, sample_interval:200000 us
   MT:0xa5a, ST:0x5a7, EW:0x3, CRC:0x799174a, sample_tm1:289221368, sample_tm2:253221299, sample_interval:200000 us
   MT:0xa5a, ST:0x5a8, EW:0x3, CRC:0x7991746, sample_tm1:325221437, sample_tm2:289221368, sample_interval:200000 us
   MT:0xa5a, ST:0x5a9, EW:0x3, CRC:0x7991701, sample_tm1:361221506, sample_tm2:325221437, sample_interval:200000 us
   MT:0xa5a, ST:0x5aa, EW:0x3, CRC:0x799170d, sample_tm1:397221575, sample_tm2:361221506, sample_interval:200000 us
   MT:0xa5a, ST:0x5ab, EW:0x3, CRC:0x7991719, sample_tm1:433221645, sample_tm2:397221575, sample_interval:200000 us
   MT:0xa5a, ST:0x5ac, EW:0x3, CRC:0x7991715, sample_tm1:469221714, sample_tm2:433221645, sample_interval:200000 us
   MT:0xa5a, ST:0x5ad, EW:0x3, CRC:0x7991731, sample_tm1:505221783, sample_tm2:469221714, sample_interval:200000 us
   MT:0xa5a, ST:0x5ae, EW:0x3, CRC:0x799173d, sample_tm1:541221852, sample_tm2:505221783, sample_interval:200000 us
   MT:0xa5a, ST:0x5af, EW:0x3, CRC:0x7991729, sample_tm1:577221921, sample_tm2:541221852, sample_interval:200000 us
   MT:0xa5a, ST:0x5b0, EW:0x3, CRC:0x7991725, sample_tm1:613221990, sample_tm2:577221921, sample_interval:200000 us
   MT:0xa5a, ST:0x5b1, EW:0x3, CRC:0x79917a7, sample_tm1:649222059, sample_tm2:613221990, sample_interval:200000 us
   MT:0xa5a, ST:0x5b2, EW:0x3, CRC:0x79917ab, sample_tm1:685222128, sample_tm2:649222059, sample_interval:200000 us
   MT:0xa5a, ST:0x5b3, EW:0x3, CRC:0x79917bf, sample_tm1:721222197, sample_tm2:685222128, sample_interval:200000 us
   MT:0xa5a, ST:0x5b4, EW:0x3, CRC:0x79917b3, sample_tm1:757222267, sample_tm2:721222197, sample_interval:200000 us
   MT:0xa5a, ST:0x5b5, EW:0x3, CRC:0x7991797, sample_tm1:793222336, sample_tm2:757222267, sample_interval:200000 us
   MT:0xa5a, ST:0x5b6, EW:0x3, CRC:0x799179b, sample_tm1:829222405, sample_tm2:793222336, sample_interval:200000 us
   MT:0xa5a, ST:0x5b7, EW:0x3, CRC:0x799178f, sample_tm1:865222474, sample_tm2:829222405, sample_interval:200000 us

