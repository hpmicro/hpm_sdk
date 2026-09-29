.. _audio_wav_decoder:

Audio Wav Decoder
==================================

Overview
--------

The audio codec sample project shows how to play WAV format audio.
In this project, the wav format file from the SD card is read and then played,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

Project Configuration
-----------------------

- The audio output device is auto-detected when this sample's ``app.yaml`` declares a ``board_audio_codec`` dependency and the board declares both ``board_audio_codec`` and a ``board_codec_*`` feature. Otherwise DAO is used.

- To override the auto-detection, set ``CONFIG_CODEC`` before ``find_package`` in ``CMakeLists.txt``:

  - ``set(CONFIG_CODEC 1)`` — force Codec output
  - ``set(CONFIG_CODEC 0)`` — force DAO output

- The Audio Codec type is automatically matched with the hardware based on the board feature (e.g., ``board_codec_es8389``). To specify an audio codec type manually, set the following in ``CMakeLists.txt``: ``set(CONFIG_CODEC_NAME "wm8960")``.

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

   audio codec example
   SD card has been mounted successfully


   ***********Music List**********

   **Enter any non-numeric key to change pages or music number to choose music**


   ***********Page**********
   0: music1.wav

   1: music2.wav

   2: music3.wav

   3: music4.wav

   4: music5.wav

   5: music6.wav

   6: mucsic7.wav

   7: Ring10.wav

   8: on_phone.wav

   9: calling.wav
