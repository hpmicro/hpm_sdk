.. _smix:

SMIX
========

Overview
--------

SMIX example project demonstrates the mixing function of audio stream. In the example project, two different sampling rates of audio are converted into the same sampling rate and mixed for playback.

Project Configuration
---------------------

- In the file `CMakeLists.txt`, set a matched audio codec type according to the development board schematic，e.g. "set(CONFIG_CODEC "wm8960")"

Board Setting
-------------

- Connect headphone to the  :ref:`headphone <board_resource>`  interface on the audio codec chip to play audio data

Known Issue
-----------

- When using the headphone interface of Codec to play audio on some development boards, there may be crosstalk. For example, when playing mono audio through the left channel of the headphone, weak sound can be heard on the right channel headphone.

Running the example
-------------------

When the project runs correctly, the serial port terminal will output the following information and the headphone will play the sound:

.. code-block:: console

   Sound mixer example
   mixer play one sound
   input sound sample rate: 8000Hz
   output sound sample rate: 48000Hz
   mixer play one sound
   input sound sample rate: 16000Hz
   output sound sample rate: 48000Hz
   mixer play two sound
   input sound0 sample rate: 8000Hz
   input sound1 sample rate: 16000Hz
   output sound sample rate: 48000Hz

