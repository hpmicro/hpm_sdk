.. _sei_master_connect_ssi_encoder:

SEI Master: Connect SSI Encoder
==============================================================

Overview
----------

This demo shows SEI interface read SSI encoder position data and automatically calculate sampling/update delay time, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Slave sample path is "samples/drivers/sei/slave/ssi", master is this example.

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
   SEI master SSI sample
   Started sei engine!
   MT:0xa5a, ST:0x5a5, sample_tm:330792, update_tm:335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a6, sample_tm:36330792, update_tm:36335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a7, sample_tm:72330792, update_tm:72335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a8, sample_tm:108330792, update_tm:108335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a9, sample_tm:144330792, update_tm:144335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5aa, sample_tm:180330792, update_tm:180335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ab, sample_tm:216330792, update_tm:216335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ac, sample_tm:252330792, update_tm:252335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ad, sample_tm:288330792, update_tm:288335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ae, sample_tm:324330792, update_tm:324335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5af, sample_tm:360330792, update_tm:360335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b0, sample_tm:396330792, update_tm:396335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b1, sample_tm:432330792, update_tm:432335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b2, sample_tm:468330792, update_tm:468335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b3, sample_tm:504330792, update_tm:504335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b4, sample_tm:540330792, update_tm:540335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b5, sample_tm:576330792, update_tm:576335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b6, sample_tm:612330792, update_tm:612335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b7, sample_tm:648330792, update_tm:648335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b8, sample_tm:684330792, update_tm:684335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b9, sample_tm:720330792, update_tm:720335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ba, sample_tm:756330792, update_tm:756335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5bb, sample_tm:792330792, update_tm:792335383, TimeDelay:255*0.1us

