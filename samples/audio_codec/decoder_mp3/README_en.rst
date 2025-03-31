.. _audio_mp3_decoder:

Audio MP3 Decoder
==================================

Overview
--------

The audio codec sample project shows how to play mp3 format audio.
In this project, the mp3 format file from the SD card is read and then played,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

Project Configuration
---------------------

- In the file `CMakeLists.txt`, set a matched audio codec type according to the development board schematic，e.g. "set(CONFIG_CODEC "sgtl5000")"

Board Setting
-------------

- According to project configuration, connect speaker to  :ref:`DAO <board_resource>`  interface if using DAO as player, connect headphone to  :ref:`headphone <board_resource>`  interface if using audio codec as player.

Known issues
------------

When using DAO to play some audio, it may generate noise.

Running the example
-------------------

When the project runs correctly, select the music name and the headset will play this music, while the serial terminal will output the following message:


.. code-block:: console

   audio codec mp3 example
   SD card has been mounted successfully


   ***********Music List**********

   **Enter any non-numeric key to change pages or music number to choose music**


   ***********Page**********
   0: 蔡琴 - 渡口.mp3

   1: BEYOND - 海阔天空.mp3

   2: Céline Dion、James Horner - My Heart Will Go On.mp3

   3: M2M - Pretty Boy.mp3

   4: Mariah Carey - Hero.mp3

   5: Michael Learns to Rock - Take Me To Your Heart.mp3

   6: Westlife - My Love.mp3

