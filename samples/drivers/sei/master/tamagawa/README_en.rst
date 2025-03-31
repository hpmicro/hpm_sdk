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
   MT:0, ST:0x3bc029, ALMC:0xc8, CRC:0xec, sample_tm:600368065, update_tm:600377411, TimeDelay:467*0.1us
   MT:0, ST:0x52c183, ALMC:0xc8, CRC:0xaf, sample_tm:640368065, update_tm:640377410, TimeDelay:467*0.1us
   MT:0, ST:0x7de1bf, ALMC:0xc8, CRC:0x63, sample_tm:680368065, update_tm:680377408, TimeDelay:467*0.1us
   MT:0x1, ST:0x104f00, ALMC:0xc8, CRC:0xdd, sample_tm:720368065, update_tm:720377408, TimeDelay:467*0.1us
   MT:0x1, ST:0x51a04c, ALMC:0xc8, CRC:0x9a, sample_tm:760368065, update_tm:760377426, TimeDelay:468*0.1us
   MT:0x1, ST:0x576bc0, ALMC:0xc8, CRC:0x18, sample_tm:800368065, update_tm:800377424, TimeDelay:467*0.1us
   MT:0x2, ST:0x461e, ALMC:0xc8, CRC:0xfd, sample_tm:840368065, update_tm:840377422, TimeDelay:467*0.1us
   MT:0x2, ST:0x461d, ALMC:0xc8, CRC:0x3d, sample_tm:880368065, update_tm:880377419, TimeDelay:467*0.1us
   MT:0x2, ST:0x461c, ALMC:0xc8, CRC:0xbd, sample_tm:920368065, update_tm:920377414, TimeDelay:467*0.1us
   MT:0x2, ST:0x461e, ALMC:0xc8, CRC:0xfd, sample_tm:960368065, update_tm:960377414, TimeDelay:467*0.1us

