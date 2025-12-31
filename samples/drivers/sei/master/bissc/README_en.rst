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
   MT:0xa5a, ST:0xfa500000, EW:0x3, CRC:0x19, sample_tm:391973, update_tm:399174, TimeDelay:360*0.1us
   delta: 1275 ns
   MT:0xa5a, ST:0xfa600000, EW:0x3, CRC:0xd, sample_tm:40391973, update_tm:40399174, TimeDelay:360*0.1us
   delta: 1275 ns
   rx point changed!
   MT:0xa5a, ST:0xfa700000, EW:0x3, CRC:0x1, sample_tm:80391973, update_tm:80399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfa800000, EW:0x3, CRC:0x6, sample_tm:120391973, update_tm:120399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfa900000, EW:0x3, CRC:0xa, sample_tm:160391973, update_tm:160399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfaa00000, EW:0x3, CRC:0x1e, sample_tm:200391973, update_tm:200399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfab00000, EW:0x3, CRC:0x12, sample_tm:240391973, update_tm:240399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfac00000, EW:0x3, CRC:0x36, sample_tm:280391973, update_tm:280399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfad00000, EW:0x3, CRC:0x3a, sample_tm:320391973, update_tm:320399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfae00000, EW:0x3, CRC:0x2e, sample_tm:360391973, update_tm:360399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfaf00000, EW:0x3, CRC:0x22, sample_tm:400391973, update_tm:400399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb000000, EW:0x3, CRC:0x20, sample_tm:440391973, update_tm:440399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb100000, EW:0x3, CRC:0x2c, sample_tm:480391973, update_tm:480399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb200000, EW:0x3, CRC:0x38, sample_tm:520391973, update_tm:520399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb300000, EW:0x3, CRC:0x34, sample_tm:560391973, update_tm:560399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb400000, EW:0x3, CRC:0x10, sample_tm:600391973, update_tm:600399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb500000, EW:0x3, CRC:0x1c, sample_tm:640391973, update_tm:640399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb600000, EW:0x3, CRC:0x8, sample_tm:680391973, update_tm:680399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb700000, EW:0x3, CRC:0x4, sample_tm:720391973, update_tm:720399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb800000, EW:0x3, CRC:0x3, sample_tm:760391973, update_tm:760399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb900000, EW:0x3, CRC:0xf, sample_tm:800391973, update_tm:800399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfba00000, EW:0x3, CRC:0x1b, sample_tm:840391973, update_tm:840399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbb00000, EW:0x3, CRC:0x17, sample_tm:880391973, update_tm:880399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbc00000, EW:0x3, CRC:0x33, sample_tm:920391973, update_tm:920399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbd00000, EW:0x3, CRC:0x3f, sample_tm:960391973, update_tm:960399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbe00000, EW:0x3, CRC:0x2b, sample_tm:1000391973, update_tm:1000399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbf00000, EW:0x3, CRC:0x27, sample_tm:1040391973, update_tm:1040399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc000000, EW:0x3, CRC:0x3b, sample_tm:1080391973, update_tm:1080399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc100000, EW:0x3, CRC:0x37, sample_tm:1120391973, update_tm:1120399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc200000, EW:0x3, CRC:0x23, sample_tm:1160391973, update_tm:1160399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc300000, EW:0x3, CRC:0x2f, sample_tm:1200391973, update_tm:1200399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc400000, EW:0x3, CRC:0xb, sample_tm:1240391973, update_tm:1240399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc500000, EW:0x3, CRC:0x7, sample_tm:1280391973, update_tm:1280399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc600000, EW:0x3, CRC:0x13, sample_tm:1320391973, update_tm:1320399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc700000, EW:0x3, CRC:0x1f, sample_tm:1360391973, update_tm:1360399174, TimeDelay:360*0.1us

