.. _audio_v2_speaker_fb:

Audio V2 Speaker Feedback
==========================================

Overview
--------

This example project shows USB Audio V2 speaker device, that uses feedback endpoints for synchronization.

- PC sees a CherryUSB speaker via Device Manager

Features
--------

- USB Audio Class 2.0.
- Two-channel, 24-bit speaker stream using 4-byte audio slots.
- Sample rates: 96 kHz, 48 kHz, and 16 kHz.
- Asynchronous speaker OUT stream with an explicit feedback endpoint.
- Runtime DMA slot length and feedback adjustment period derived from the USB service interval.

Project Configuration
-----------------------

- The audio output device is auto-detected when ``app.yaml`` declares a ``board_audio_codec`` dependency and the board declares both ``board_audio_codec`` and a ``board_codec_*`` feature. Otherwise DAO is used.

- To override the auto-detection, set ``CONFIG_CODEC`` before ``find_package`` in ``CMakeLists.txt``:

  - ``set(CONFIG_CODEC 1)`` — force Codec output
  - ``set(CONFIG_CODEC 0)`` — force DAO output

- The Audio Codec type is automatically matched with the hardware based on the board feature (e.g., ``board_codec_es8389``). To specify an audio codec type manually, set the following in ``CMakeLists.txt``: ``set(CONFIG_CODEC_NAME "wm8960")``.

- The USB High Speed and Full Speed service intervals are configured by ``EP_INTERVAL_HS`` and ``EP_INTERVAL_FS``. The packets-per-second value, USB maximum packet size, runtime DMA slot length, and feedback adjustment period are derived automatically from the configured interval.

- The USB descriptor packet size is the nominal service-interval slot plus one audio frame. When the stream is opened, the example also verifies that the circular buffer capacity is an integer multiple of the runtime DMA slot length.


Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

- Connect headphone to  :ref:`headphone <board_resource>`  interface (Audio Codec is the default playback device), or connect speaker to  :ref:`DAO <board_resource>`  interface if DAO is configured.

Running the example
-------------------

- Download the program and run. The computer can automatically recognize and install the USB audio driver and enumerate a device with a speaker device.

- Select the speaker device as the default player, and the PC will play audio through the headphone (Codec) interface or DAO interface depending on the configuration.

- Change the sample rate while the streaming interface is inactive, then reopen the interface so that the runtime DMA slot length and nominal feedback value are recalculated.

- During playback, feedback synchronization status information is printed periodically via the serial console in the following format:

  .. code-block:: console

     fb: used=12288/24576 (50%) val=786432 hz=96000

  Field descriptions:

  - ``used``: Amount of data currently buffered in the circular buffer (bytes)
  - ``total``: Total capacity of the circular buffer (bytes)
  - ``percent``: Buffer usage percentage, reflecting the rate synchronization between device and host
  - ``val``: Raw calculated feedback value transmitted through the explicit feedback endpoint
  - ``hz``: Sample rate represented by the feedback value

  A percentage close to 50% indicates good rate synchronization between device and host. The feedback controller is updated approximately every 100 ms. Consistently high values cause the device to request a lower host transmit rate; consistently low values cause it to request a higher host transmit rate.

  With the default HS ``bInterval=4`` or FS ``bInterval=1``, both modes use 1000 service intervals per second. At 96 kHz, two channels, and 4-byte slots, the nominal DMA slot is 768 bytes and the 32-slot circular buffer is 24576 bytes.
