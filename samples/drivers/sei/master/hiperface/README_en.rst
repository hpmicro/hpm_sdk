.. _sei_master_connect_hiperface_encoder:

SEI Master: Connect HIPERFACE Encoder
==========================================================================

Overview
----------

This demo shows SEI interface read HIPERFACE encoder position data and automatically calculate sampling/update delay time, then print related information by uart console.

configuration
---------------

- HIPERFACE encoder is SKM36-HFA0-K02.

- Connect master's SEI pins DATA_P/DATA_N to encoder data pins.

- Connect encoder VDD/GND to an external power supply +9V.

Running the example
-------------------

- When the example runs successfully, the console shows the following log. Rotate the encoder and the POS value will change.


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
   SEI master hiperface sample
   Started sei engine!
   rev:0x272, pos:0x55100000, addr:0x40, CRC:0x8a, sample_tm:2683013340, update_tm:2684690833, TimeDelay:8387 us
   rev:0x272, pos:0xa400000, addr:0x40, CRC:0x85, sample_tm:2723013856, update_tm:2724691348, TimeDelay:8387 us
   rev:0x272, pos:0xa000000, addr:0x40, CRC:0xa5, sample_tm:2763013939, update_tm:2764691431, TimeDelay:8387 us
   rev:0x271, pos:0xc0200000, addr:0x40, CRC:0xdc, sample_tm:2803013180, update_tm:2804690667, TimeDelay:8387 us
   rev:0x271, pos:0x7f900000, addr:0x40, CRC:0xd3, sample_tm:2843013262, update_tm:2844690749, TimeDelay:8387 us
   rev:0x271, pos:0x6f800000, addr:0x40, CRC:0xd3, sample_tm:2883012911, update_tm:2884690397, TimeDelay:8387 us
   rev:0x271, pos:0x6f600000, addr:0x40, CRC:0xa3, sample_tm:2923014268, update_tm:2924691755, TimeDelay:8387 us
   rev:0x271, pos:0x6fa00000, addr:0x40, CRC:0x93, sample_tm:2963013510, update_tm:2964690996, TimeDelay:8387 us
   rev:0x271, pos:0x6f900000, addr:0x40, CRC:0x53, sample_tm:3003013158, update_tm:3004690645, TimeDelay:8387 us
   rev:0x271, pos:0x6fa00000, addr:0x40, CRC:0x93, sample_tm:3043013784, update_tm:3044691270, TimeDelay:8387 us
   rev:0x271, pos:0x6fa00000, addr:0x40, CRC:0x93, sample_tm:3083016146, update_tm:3084693633, TimeDelay:8387 us
   rev:0x271, pos:0x6fa00000, addr:0x40, CRC:0x93, sample_tm:3123011969, update_tm:3124689456, TimeDelay:8387 us

