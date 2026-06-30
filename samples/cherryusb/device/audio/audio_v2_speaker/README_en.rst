.. _audio_v2_speaker:

Audio V2 Speaker
================================

Overview
--------

This example project shows USB Audio V2 speaker device.

- PC sees a CherryUSB speaker via Device Manager

Project Configuration
-----------------------

- In the `CMakeLists.txt` file, configure the type of audio playback device according to requirements. The default playback uses DAO.

- If using the onboard Audio Codec chip, the Audio Codec type is automatically matched with the hardware based on the feature settings in `boards/<board_name>/<board_name>.yaml` (feature: board_codec_xxxx) by default. To specify an audio codec type, set the following in `CMakeLists.txt`: set(CONFIG_CODEC_NAME "wm8960").


Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

- According to project configuration, connect speaker to  :ref:`DAO <board_resource>`  interface if using DAO as player, connect headphone to  :ref:`headphone <board_resource>`  interface if using audio codec as player.

Running the example
-------------------

- Download the program and run. The computer can automatically recognize and install the USB audio driver and enumerate a device with a speaker device.

- Select the speaker device as the default player, and the PC will play audio through the DAO interface
