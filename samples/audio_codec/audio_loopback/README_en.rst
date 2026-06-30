.. _audio_codec_loopback:

Audio Codec Loopback
========================================

Overview
--------

The audio_codec example shows how to use codec component to record and play audio information.
In this example, codec component records the sound and sents out encoded data by I2S interface,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

Project Configuration
-----------------------

- The Audio Codec type is automatically matched with the hardware based on the feature settings in `boards/<board_name>/<board_name>.yaml` (feature: board_codec_xxxx) by default. To specify an audio codec type, set the following in `CMakeLists.txt`: set(CONFIG_CODEC_NAME "wm8960").

Board Setting
-------------

1. Choose one of the following audio input methods based on the board schematic:
    - Connect the music player to the LINE_IN interface of the onboard codec using a 3.5mm audio cable to record audio data.

    - Use the onboard codec's microphone to record audio data.

2. Connect headphone to the  :ref:`headphone <board_resource>`  interface on the audio codec chip to play audio data

Running the example
-------------------

When the example runs successfully, the headphone will play the sound and the following message is displayed in the terminal:

.. code-block:: console

   Audio Codec example
   Test Codec playback and record

