.. _sei_master_connect_nikon_encoder:

SEI Master: Connect NIKON Encoder
==================================================================

Overview
----------

This demo shows SEI interface read NIKON encoder (Product: MAR-MC42AHN00) position data and automatically calculate sampling/update delay time, then print related information by uart console.

configuration
--------------

- Two development boards are required, one as the master and one as the slave.

- Slave sample path is "samples/drivers/sei/slave/nikon", master is this example.

- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.

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
   SEI master nikon sample
   Started sei engine!
   ST:0xfffa5, MT:0x8888, CRC:0xa9, sample_tm:320369459, update_tm:320375597, TimeDelay:306*0.1us
   ST:0xfffa6, MT:0x8888, CRC:0x7a, sample_tm:360369459, update_tm:360375597, TimeDelay:306*0.1us
   ST:0xfffa7, MT:0x8888, CRC:0x59, sample_tm:400369459, update_tm:400375598, TimeDelay:306*0.1us
   ST:0xfffa8, MT:0x8888, CRC:0xca, sample_tm:440369459, update_tm:440375597, TimeDelay:306*0.1us
   ST:0xfffa9, MT:0x8888, CRC:0xe9, sample_tm:480369459, update_tm:480375597, TimeDelay:306*0.1us
   ST:0xfffaa, MT:0x8888, CRC:0x55, sample_tm:520369459, update_tm:520375597, TimeDelay:306*0.1us
   ST:0xfffab, MT:0x8888, CRC:0x76, sample_tm:560369459, update_tm:560375598, TimeDelay:306*0.1us
   ST:0xfffac, MT:0x8888, CRC:0xb, sample_tm:600369459, update_tm:600375598, TimeDelay:306*0.1us

