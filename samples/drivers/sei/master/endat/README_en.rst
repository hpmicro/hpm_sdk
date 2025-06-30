.. _sei_master_connect_endat_encoder:

SEI Master: Connect ENDAT Encoder
==================================================================

Overview
----------

This demo shows SEI interface read Endat encoder position data. Bus delay detection is achieved by latch the sampling time and latch the data edge time, and automatically calculate reasonable rx points. Also, it is automatically calculate sampling/update delay time, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Slave sample path is "samples/drivers/sei/slave/endat", master is this example.

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
   SEI master EnDat sample
   Started sei engine!
   delta: 15790 ns
   E:0x0, ST:0xa5a5, CRC:0x5, sample_tm:365657, update_tm:375158, TimeDelay:475*0.1us
   delta: 15790 ns
   E:0x0, ST:0xa5a6, CRC:0xf, sample_tm:40365657, update_tm:40375158, TimeDelay:475*0.1us
   delta: 15795 ns
   rx point changed!
   E:0, ST:0xa5a7, CRC:0x1a, sample_tm:2545400001, update_tm:2545409502, TimeDelay:475*0.1us
   E:0, ST:0xa5a8, CRC:0x7, sample_tm:2585400001, update_tm:2585409502, TimeDelay:475*0.1us
   E:0, ST:0xa5a9, CRC:0x12, sample_tm:2625400001, update_tm:2625409502, TimeDelay:475*0.1us
   E:0, ST:0xa5aa, CRC:0x18, sample_tm:2665400001, update_tm:2665409502, TimeDelay:475*0.1us

