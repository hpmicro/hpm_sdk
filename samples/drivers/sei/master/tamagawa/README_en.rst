.. _sei_master_connect_tamagawa_encoder:

SEI Master: Connect TAMAGAWA Encoder
========================================================================

Overview
----------

This demo shows SEI interface read TAMAGAWA encoder position data and automatically calculate sampling/update delay time, then print related information by uart console.

configuration
--------------

- HIPERFACE encoder is TS5700N8401.

- Connect master's SEI pins DATA_P/DATA_N to encoder data pins.

- Connect encoder VDD/GND to board +5V/GND.

Running the example
-------------------

- When the example runs successfully, the console shows the following log. Rotate the encoder and the ST and MT value will change.


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
   SEI master tamagawa sample
   Started sei engine!
   MT:0x8888, ST:0xa5a5, ALMC:0, CRC:0xd, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a6, ALMC:0, CRC:0xe, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a7, ALMC:0, CRC:0xf, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a8, ALMC:0, CRC:0, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a9, ALMC:0, CRC:0x1, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5aa, ALMC:0, CRC:0x2, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ab, ALMC:0, CRC:0x3, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ac, ALMC:0, CRC:0x4, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ad, ALMC:0, CRC:0x5, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ae, ALMC:0, CRC:0x6, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5af, ALMC:0, CRC:0x7, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b0, ALMC:0, CRC:0x18, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b1, ALMC:0, CRC:0x19, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b2, ALMC:0, CRC:0x1a, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b3, ALMC:0, CRC:0x1b, TimeDelta:462*0.1us

