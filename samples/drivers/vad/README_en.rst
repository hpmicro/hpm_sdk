.. _vad:

VAD
======

Overview
--------

The example shows recording sound with VAD and playing sound with DAO.

Board Setting
-------------

Connect speaker to DAO interface on board.

Known issues
------------

When using DAO to play some audio, it may generate noise.

Running the example
-------------------

After downloading the program, press the RESET button to reset and run program(the VAD peripheral is located in the power management domain, the RESET button reset(global reset) can reset the VAD peripheral), make some sound according to the log prompt, the sound will be recorded and played back. The following message is displayed in the terminal:

.. code-block:: console

   vad example
   vad calibration done
   Recording...
   Please make some sound to mic0
   Got something...
   Recorded 204800 bytes data
   Now, playing...

