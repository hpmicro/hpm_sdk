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
   MT:0xa5a, ST:0xfa5, sample_tm:388891, update_tm:393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa6, sample_tm:40388891, update_tm:40393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa7, sample_tm:80388891, update_tm:80393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa8, sample_tm:120388891, update_tm:120393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa9, sample_tm:160388891, update_tm:160393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfaa, sample_tm:200388891, update_tm:200393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfab, sample_tm:240388891, update_tm:240393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfac, sample_tm:280388891, update_tm:280393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfad, sample_tm:320388891, update_tm:320393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfae, sample_tm:360388891, update_tm:360393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfaf, sample_tm:400388891, update_tm:400393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb0, sample_tm:440388891, update_tm:440393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb1, sample_tm:480388891, update_tm:480393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb2, sample_tm:520388891, update_tm:520393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb3, sample_tm:560388891, update_tm:560393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb4, sample_tm:600388891, update_tm:600393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb5, sample_tm:640388891, update_tm:640393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb6, sample_tm:680388891, update_tm:680393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb7, sample_tm:720388891, update_tm:720393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb8, sample_tm:760388891, update_tm:760393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb9, sample_tm:800388891, update_tm:800393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfba, sample_tm:840388891, update_tm:840393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbb, sample_tm:880388891, update_tm:880393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbc, sample_tm:920388891, update_tm:920393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbd, sample_tm:960388891, update_tm:960393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbe, sample_tm:1000388891, update_tm:1000393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbf, sample_tm:1040388891, update_tm:1040393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfc0, sample_tm:1080388891, update_tm:1080393992, TimeDelay:255*0.1us


