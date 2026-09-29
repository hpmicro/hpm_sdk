.. _audio_v2_mic_adaptive:

Audio V2 Microphone Adaptive
======================================

Overview
--------

This example project shows USB Audio V2 microphone device.

- PC sees a CherryUSB microphone via Device Manager
- The microphone adaptively changes the USB IN packet length according to the recording buffer level.

Features
--------

- USB Audio Class 2.0.
- Two-channel, 24-bit microphone stream using 4-byte audio slots.
- Codec sample rates: 96 kHz, 48 kHz, and 16 kHz.
- PDM sample rate: 16 kHz.
- Adaptive USB IN packet length adjustment around the center of the recording buffer.

Project Configuration
-----------------------

- The audio input device is auto-detected when ``app.yaml`` declares a ``board_audio_codec`` dependency and the board declares both ``board_audio_codec`` and a ``board_codec_*`` feature. Otherwise PDM is used.

- To override the auto-detection, set ``CONFIG_CODEC`` before ``find_package`` in ``CMakeLists.txt``:

  - ``set(CONFIG_CODEC 1)`` — force Codec input
  - ``set(CONFIG_CODEC 0)`` — force PDM input

- The Audio Codec type is automatically matched with the hardware based on the board feature (e.g., ``board_codec_es8389``). To specify an audio codec type manually, set the following in ``CMakeLists.txt``: ``set(CONFIG_CODEC_NAME "wm8960")``.

- The USB High Speed and Full Speed service intervals are configured by ``EP_INTERVAL_HS`` and ``EP_INTERVAL_FS``. The packets-per-second value, USB maximum packet size, runtime DMA slot length, and status print period are derived automatically from the configured interval.

- The USB descriptor packet size is the nominal service-interval slot plus one audio frame, allowing the adaptive mechanism to increase a USB IN transfer by one frame when required.


Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

- Connect microphone to  :ref:`microphone <board_resource>`  interface (Audio Codec is the default input device), or use the onboard PDM microphone if PDM is configured.

Running the example
-------------------

- Download the program and run. The computer can automatically recognize and install the USB audio driver and enumerate a device with a microphone device.

- Select the microphone device as the default audio input device, and the PC will input audio through the Codec interface or PDM interface depending on the configuration.

- Change the sample rate while the streaming interface is inactive, then reopen the interface so that the runtime DMA slot length is recalculated.

- During recording, microphone buffer status information is printed periodically via the serial console. For example, a Codec configuration can print:

  .. code-block:: console

    mic buf: used=12192/24576 (49%)

  Field descriptions:

  - ``used``: Amount of data currently in the circular buffer (bytes)
  - ``total``: Total capacity of the circular buffer (bytes)
  - ``percent``: Buffer usage percentage, reflecting the rate synchronization between device and host

  The buffer capacity depends on the selected input backend, maximum sample rate, and USB service interval.

  The adaptive mechanism uses a ±12.5% adjustment window around 50%: one audio frame is added above 62.5%, one audio frame is removed below 37.5%, and the nominal packet length is used between 37.5% and 62.5%.
