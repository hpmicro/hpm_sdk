.. _audio_v2_speaker:

Audio V2 Speaker
================================

Overview
--------

This example project shows USB Audio V2 speaker device.

- PC sees a CherryUSB speaker via Device Manager

Project Configuration
-----------------------

- The audio output device is auto-detected when ``app.yaml`` declares a ``board_audio_codec`` dependency and the board declares both ``board_audio_codec`` and a ``board_codec_*`` feature. Otherwise DAO is used.

- To override the auto-detection, set ``CONFIG_CODEC`` before ``find_package`` in ``CMakeLists.txt``:

  - ``set(CONFIG_CODEC 1)`` — force Codec output
  - ``set(CONFIG_CODEC 0)`` — force DAO output

- The Audio Codec type is automatically matched with the hardware based on the board feature (e.g., ``board_codec_es8389``). To specify an audio codec type manually, set the following in ``CMakeLists.txt``: ``set(CONFIG_CODEC_NAME "wm8960")``.

- The USB High Speed and Full Speed service intervals are configured by ``EP_INTERVAL_HS`` and ``EP_INTERVAL_FS``. The packets-per-second value and USB maximum packet size are derived automatically from the configured interval.


Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

- Connect headphone to  :ref:`headphone <board_resource>`  interface (Audio Codec is the default playback device), or connect speaker to  :ref:`DAO <board_resource>`  interface if DAO is configured.

Running the example
-------------------

- Download the program and run. The computer can automatically recognize and install the USB audio driver and enumerate a device with a speaker device.

- Select the speaker device as the default player, and the PC will play audio through the headphone (Codec) interface or DAO interface depending on the configuration.
