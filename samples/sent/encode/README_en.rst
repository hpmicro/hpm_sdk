sent encode
===========

Overview
--------

- The sent_signal_encode sample project demonstrates the implementation of SENT encoding using SPI
- the example After initializing the SENT signal encoding, continuously sends SENT signals

Board Setting
-------------

-  Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
-  The SENT output signal is the SPI’s MOSI pin, which can be connected
   to the corresponding input pin of the SENT/decoder sample project for
   testing purposes or analyzed using a logic analyzer to observe the
   waveform

Notes
-----

-  By default, the generated SENT signal includes a pause signal. If the
   pause signal is not required, can redefine SENT_ENABLE_PAUSE_NIBBLE
   to 0, or add sdk_compile_definitions(-DSENT_ENABLE_PAUSE_NIBBLE=0) in
   the project’s CMakeLists.txt file.
-  The default idle polarity of the SENT signal is low. If need to
   change it to high, redefine SENT_IDLE_POLARITY to 1, or add
   sdk_compile_definitions(-DSENT_IDLE_POLARITY=1) in the project’s
   CMakeLists.txt file.

Running the example
-------------------

- Run decode first, and then run encode

- When running the project, the example execution outputs SENT signals. The serial terminal displays the following information, which corresponds to a complete frame of the SENT protocol.

.. code:: console

   sent signal encode demo
   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true
