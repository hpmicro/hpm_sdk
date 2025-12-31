.. _sei_slave_simulate_ssi_encoder:

SEI Slave: Simulate SSI Encoder
==============================================================

Overview
----------

This demo shows SEI interface simulate SSI encoder and generate position data, then print related information by uart console.

configuration
---------------

- Two development boards are required, one as the master and one as the slave.

- Master sample path is "samples/drivers/sei/master/ssi", slave is this example.

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
   SEI slave SSI sample
   Started sei engine!
   MT:0xa5a, ST:0xfa5, sample_tm1:268206022, sample_tm2:0, sample_interval:1341030 us
   MT:0xa5a, ST:0xfa6, sample_tm1:308206752, sample_tm2:268206022, sample_interval:200003 us
   MT:0xa5a, ST:0xfa7, sample_tm1:348207481, sample_tm2:308206752, sample_interval:200003 us
   MT:0xa5a, ST:0xfa8, sample_tm1:388208211, sample_tm2:348207481, sample_interval:200003 us
   MT:0xa5a, ST:0xfa9, sample_tm1:428208940, sample_tm2:388208211, sample_interval:200003 us
   MT:0xa5a, ST:0xfaa, sample_tm1:468209669, sample_tm2:428208940, sample_interval:200003 us
   MT:0xa5a, ST:0xfab, sample_tm1:508210398, sample_tm2:468209669, sample_interval:200003 us
   MT:0xa5a, ST:0xfac, sample_tm1:548211128, sample_tm2:508210398, sample_interval:200003 us
   MT:0xa5a, ST:0xfad, sample_tm1:588211857, sample_tm2:548211128, sample_interval:200003 us
   MT:0xa5a, ST:0xfae, sample_tm1:628212585, sample_tm2:588211857, sample_interval:200003 us
   MT:0xa5a, ST:0xfaf, sample_tm1:668213314, sample_tm2:628212585, sample_interval:200003 us
   MT:0xa5a, ST:0xfb0, sample_tm1:708214043, sample_tm2:668213314, sample_interval:200003 us
   MT:0xa5a, ST:0xfb1, sample_tm1:748214771, sample_tm2:708214043, sample_interval:200003 us
   MT:0xa5a, ST:0xfb2, sample_tm1:788215500, sample_tm2:748214771, sample_interval:200003 us
   MT:0xa5a, ST:0xfb3, sample_tm1:828216228, sample_tm2:788215500, sample_interval:200003 us
   MT:0xa5a, ST:0xfb4, sample_tm1:868216957, sample_tm2:828216228, sample_interval:200003 us
   MT:0xa5a, ST:0xfb5, sample_tm1:908217685, sample_tm2:868216957, sample_interval:200003 us
   MT:0xa5a, ST:0xfb6, sample_tm1:948218414, sample_tm2:908217685, sample_interval:200003 us
   MT:0xa5a, ST:0xfb7, sample_tm1:988219142, sample_tm2:948218414, sample_interval:200003 us
   MT:0xa5a, ST:0xfb8, sample_tm1:1028219871, sample_tm2:988219142, sample_interval:200003 us
   MT:0xa5a, ST:0xfb9, sample_tm1:1068220599, sample_tm2:1028219871, sample_interval:200003 us
   MT:0xa5a, ST:0xfba, sample_tm1:1108221327, sample_tm2:1068220599, sample_interval:200003 us
   MT:0xa5a, ST:0xfbb, sample_tm1:1148222055, sample_tm2:1108221327, sample_interval:200003 us
   MT:0xa5a, ST:0xfbc, sample_tm1:1188222783, sample_tm2:1148222055, sample_interval:200003 us
   MT:0xa5a, ST:0xfbd, sample_tm1:1228223512, sample_tm2:1188222783, sample_interval:200003 us
   MT:0xa5a, ST:0xfbe, sample_tm1:1268224240, sample_tm2:1228223512, sample_interval:200003 us
   MT:0xa5a, ST:0xfbf, sample_tm1:1308224968, sample_tm2:1268224240, sample_interval:200003 us
   MT:0xa5a, ST:0xfc0, sample_tm1:1348225695, sample_tm2:1308224968, sample_interval:200003 us
   MT:0xa5a, ST:0xfc1, sample_tm1:1388226423, sample_tm2:1348225695, sample_interval:200003 us
   MT:0xa5a, ST:0xfc2, sample_tm1:1428227151, sample_tm2:1388226423, sample_interval:200003 us


