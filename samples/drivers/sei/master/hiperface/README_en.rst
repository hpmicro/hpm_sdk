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
   rev:0xa, pos:0x5a500000, addr:0x40, CRC:0x2, TimeDelta:7760 us
   rev:0xa, pos:0x5a600000, addr:0x40, CRC:0x1, TimeDelta:7760 us
   rev:0xa, pos:0x5a700000, addr:0x40, CRC:0x0, TimeDelta:7760 us
   rev:0xa, pos:0x5a800000, addr:0x40, CRC:0xf, TimeDelta:7760 us
   rev:0xa, pos:0x5a900000, addr:0x40, CRC:0xe, TimeDelta:7760 us
   rev:0xa, pos:0x5aa00000, addr:0x40, CRC:0xd, TimeDelta:7760 us
   rev:0xa, pos:0x5ab00000, addr:0x40, CRC:0xc, TimeDelta:7760 us
   rev:0xa, pos:0x5ac00000, addr:0x40, CRC:0xb, TimeDelta:7760 us
   rev:0xa, pos:0x5ad00000, addr:0x40, CRC:0xa, TimeDelta:7760 us
   rev:0xa, pos:0x5ae00000, addr:0x40, CRC:0x9, TimeDelta:7760 us
   rev:0xa, pos:0x5af00000, addr:0x40, CRC:0x8, TimeDelta:7760 us
   rev:0xa, pos:0x5b000000, addr:0x40, CRC:0x17, TimeDelta:7760 us
   rev:0xa, pos:0x5b100000, addr:0x40, CRC:0x16, TimeDelta:7760 us
   rev:0xa, pos:0x5b200000, addr:0x40, CRC:0x15, TimeDelta:7760 us
   rev:0xa, pos:0x5b300000, addr:0x40, CRC:0x14, TimeDelta:7760 us
   rev:0xa, pos:0x5b400000, addr:0x40, CRC:0x13, TimeDelta:7760 us
