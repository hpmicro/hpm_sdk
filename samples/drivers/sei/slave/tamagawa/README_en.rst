.. _sei_slave_simulate_tamagawa_encoder:

SEI Slave: Simulate TAMAGAWA Encoder
========================================================================

Overview
----------

- This demo shows SEI interface simulate TAMAGAWA encoder and generate position data, then print related information by uart console.

- The simulated TAMAGAWA encoder is TS5700N8401。

Configuration
----------------

- Prepare a USB to 485 module (Note: The maximum baudrate of this module must not be less than 2.5Mbps)

- Connect SEI pins DATA_P/DATA_N to 485 module pins A/B

Running the example
-------------------

- Download and run the program in the development board.

- Use PC serial debugging assistant to communicate with the development board, obtain simulated encoder data.

- Set PC serial debugging assistant: baudrate-2500000, startbits-1, databits-8, stopbits-1, paritybits-none.

- Use PC serial debugging assistant send Hex data: `1A` or `02` or `8A` or `92`, the development board will response. Meanwhile, the console shows the following log. ST's value increasing by 1 each sample.


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
   SEI slave tamagawa sample
   Started sei engine!
   CMD:0x1a, SF:0x0, ST:0xa5a5, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x0, sample_tm1:135187399, sample_tm2:0, sample_interval:844921 us
   CMD:0x1a, SF:0x0, ST:0xa5a6, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:167187056, sample_tm2:135187399, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5a7, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:199186714, sample_tm2:167187056, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5a8, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:231186372, sample_tm2:199186714, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5a9, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:263186030, sample_tm2:231186372, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5aa, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:295185688, sample_tm2:263186030, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5ab, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:327185345, sample_tm2:295185688, sample_interval:199997 us


- Use PC serial debugging assistant send Hex data：`32 7F 01 4C`，which will set EEPROM page number to 1，the development board will response. Meanwhile, the console shows the following log.


.. code-block:: text

   Change EEPORM page to 1


- Use PC serial debugging assistant send Hex data：`32 01 5A 69`，which will write data 0x5A to address 01 of EEPROM，the development board will response. Meanwhile, the console shows the following log.


.. code-block:: text

   Write EEPORM - Page: 1, Addr: 1, Data: 90


- Use PC serial debugging assistant send Hex data：`EA 01 EB`，which will read data from address 01 of EEPROM，the development board will response. Meanwhile, the console shows the following log.


.. code-block:: text

   Read EEPORM - Page: 1, Addr: 1, Data: 90

