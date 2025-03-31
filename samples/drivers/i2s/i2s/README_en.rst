.. _i2s:

I2S
======

Overview
--------

The example shows recording and playing sound by i2s interface(PDM and DAO peripheral).

Board Setting
-------------

Connect speaker to DAO interface on board.

Known issues
------------

When using DAO to play some audio, it may generate noise.

Running the example
-------------------

When the example runs successfully, make some sound according to the log prompt, the sound will be recorded and played back. The following message is displayed in the terminal:

.. code-block:: console

   i2s example
   testing pdm to dao
   Recording...
   Please make some sound to mic0
   Playing...
   Recording...
   Please make some sound to mic0
   testing wav playback
   testing sine wave playback

