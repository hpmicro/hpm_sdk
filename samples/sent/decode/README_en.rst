sent decode
===========

Overview
--------

-  The example shows the measure PWM wave of gptmr and uses DMA to collect and decode sent signal

- After initializing the SENT signal decoding instance, it continuously waits for SENT signals from the encoder

- The pwm_process_sent API function within the example decodes SENT signals. It returns success upon parsing a complete frame of SENT signal; if the signal is incomplete or not a SENT signal, it returns an error

- The example outputs only successfully decoded SENT signals via UART. Failed decoding attempts are filtered out, and the system continues to wait for the next complete SENT signal.

Board Setting
-------------

-  Please refer to `Pin Description <board_resource>`__ for specific
   board.
-  Connect external SENT signal to GPTMR capture pin.The SENT signal can
   be generated using the sent/encode example.

Running the example
-------------------
- Run decode first, and then run encode.

- Running the example,if there is a SENT signal input, the serial terminal outputs the following information,which are the sent protocol frame

.. code:: console

   sent signal decode demo
   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

