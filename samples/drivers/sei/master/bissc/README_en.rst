.. _sei_master_connect_bissc_encoder:

SEI Master: Connect BISSC Encoder
==================================================================

Overview
----------

This demo shows SEI interface read BISSC encoder position data. Bus delay detection is achieved by latch the sampling time and latch the data edge time, and automatically calculate reasonable rx points. Also, it is automatically calculate sampling/update delay time, then print related information by uart console.

configuration
---------------

- Two development boards are required, one as the master and one as the slave.

- Slave sample path is "samples/drivers/sei/slave/bissc", master is this example.

- Master's SEI_CLK pins should bridging master side，and slave's SEI_CLK pins should bridging slave side.

- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.

- Connect master's SEI pins CLKO_P/CLKO_N to slave's SEI pins CLKI_P/CLKI_N.

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
   SEI master BiSS_C sample
   Started sei engine!
   delta: 1280 ns
   MT:0xa5a, ST:0x5a500000, EW:0x3, CRC:0x799175e, sample_tm:365810, update_tm:373011, TimeDelay:360*0.1us
   delta: 1280 ns
   MT:0xa5a, ST:0x5a600000, EW:0x3, CRC:0x799174a, sample_tm:40365810, update_tm:40373011, TimeDelay:360*0.1us
   delta: 1280 ns
   rx point changed!
   MT:0xa5a, ST:0x5a700000, EW:0x3, CRC:0x7991746, sample_tm:72330604, update_tm:72337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5a800000, EW:0x3, CRC:0x7991701, sample_tm:108330604, update_tm:108337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5a900000, EW:0x3, CRC:0x799170d, sample_tm:144330604, update_tm:144337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5aa00000, EW:0x3, CRC:0x7991719, sample_tm:180330604, update_tm:180337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5ab00000, EW:0x3, CRC:0x7991715, sample_tm:216330604, update_tm:216337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5ac00000, EW:0x3, CRC:0x7991731, sample_tm:252330604, update_tm:252337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5ad00000, EW:0x3, CRC:0x799173d, sample_tm:288330604, update_tm:288337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5ae00000, EW:0x3, CRC:0x7991729, sample_tm:324330604, update_tm:324337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5af00000, EW:0x3, CRC:0x7991725, sample_tm:360330604, update_tm:360337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b000000, EW:0x3, CRC:0x79917a7, sample_tm:396330604, update_tm:396337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b100000, EW:0x3, CRC:0x79917ab, sample_tm:432330604, update_tm:432337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b200000, EW:0x3, CRC:0x79917bf, sample_tm:468330604, update_tm:468337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b300000, EW:0x3, CRC:0x79917b3, sample_tm:504330604, update_tm:504337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b400000, EW:0x3, CRC:0x7991797, sample_tm:540330604, update_tm:540337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b500000, EW:0x3, CRC:0x799179b, sample_tm:576330604, update_tm:576337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b600000, EW:0x3, CRC:0x799178f, sample_tm:612330604, update_tm:612337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b700000, EW:0x3, CRC:0x7991783, sample_tm:648330604, update_tm:648337085, TimeDelay:360*0.1us
   MT:0xa5a, ST:0x5b800000, EW:0x3, CRC:0x79917c4, sample_tm:684330604, update_tm:684337085, TimeDelay:360*0.1us

