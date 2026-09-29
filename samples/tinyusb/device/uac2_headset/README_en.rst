.. _uac2_headset:

UAC2 Headset
======================================================

Overview
--------

This example implements a USB audio headset with stereo speaker and microphone functions.

- Audio playback data sent by the PC is output through the board I2S interface and audio codec.

- Audio captured by the board audio codec is sent to the PC as microphone data.

- The headset uses a 48 kHz, stereo, 24-bit audio format in 32-bit slots. It enumerates as UAC2 at high speed and as UAC1 at full speed.

- Speaker and microphone mute and volume can be controlled from the host. When the device operates as UAC1, the on-board button switches the speaker volume between 10% and 100%.

Board Setting
-------------

- Connect the PWR DEBUG port on the development board to a USB port on the PC with a USB Type-C cable.

- Connect the USB device port selected by ``BOARD_TUD_RHPORT`` to a USB port on the PC with a USB Type-C cable.

- Connect a speaker or headphones and a microphone to the board audio codec interface.

Project Configuration
---------------------

- ``BOARD_TUD_RHPORT`` in ``src/tusb_config.h`` selects the USB device controller. Set it to ``0`` for USB0 or ``1`` for USB1 on boards that provide USB1. The default is USB0.

- USB audio format, endpoint interval, and buffer sizes can be configured in ``src/tusb_config.h``. If these settings are changed, update the USB descriptors accordingly.

- The example requires a board with an I2S audio codec, as indicated by its ``i2s`` and ``board_audio_codec`` dependencies.

Running the example
-------------------

When the example runs correctly, the serial console prints:

.. code-block:: console

   Headset running

1. Open the PC sound settings and select ``TinyUSB Headset`` as the playback device and recording device.

2. Play audio on the PC. The audio is output through the speaker or headphones connected to the board.

3. Open a recording application on the PC and select ``TinyUSB Headset`` as the input device. Speak into the microphone connected to the board, then record and play back the captured audio.

4. Adjust the playback or microphone volume, or mute either path, in the PC sound settings to verify the audio controls.
