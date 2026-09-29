.. _audio_v2_headset:

Audio V2 Headset
================

Overview
--------

This example implements a USB Audio V2 headset with simultaneous speaker playback and microphone recording:

- The speaker uses an explicit feedback endpoint to regulate the host transmit rate and prevent playback buffer overflow or underflow.
- The microphone adaptively changes the USB IN packet length according to the recording buffer level.
- The PC enumerates a CherryUSB headset containing speaker and microphone audio endpoints.

Features
--------

- USB Audio Class 2.0.
- Two-channel, 24-bit speaker and microphone streams using 4-byte audio slots.
- High Speed sample rates: 192 kHz, 96 kHz, 48 kHz, 32 kHz, and 16 kHz.
- Full Speed sample rates: 96 kHz, 48 kHz, and 16 kHz.
- The speaker and microphone share the Audio Codec clock source and sample rate.
- Asynchronous speaker OUT stream with explicit feedback synchronization.
- Adaptive microphone IN packet length adjustment.

Project Configuration
---------------------

- This example requires the target board to provide the ``board_audio_codec`` feature.
- The Audio Codec type is automatically selected from the ``board_codec_*`` feature in ``boards/<board_name>/<board_name>.yaml``, for example ``board_codec_es8389``, ``board_codec_wm8960``, or ``board_codec_sgtl5000``.
- To select an Audio Codec manually, set ``CONFIG_CODEC_NAME`` before ``find_package`` in ``CMakeLists.txt``:

  .. code-block:: cmake

     set(CONFIG_CODEC_NAME "wm8960")

- The USB High Speed and Full Speed service intervals are configured by ``EP_INTERVAL_HS`` and ``EP_INTERVAL_FS``. The packets-per-second value, USB maximum packet size, and DMA transfer length are derived automatically from the configured interval.
- When a stream is opened, its nominal DMA slot length is calculated from the current sample rate, audio frame size, and packets-per-second value. The USB descriptor packet size reserves one additional audio frame for feedback or adaptive adjustment.
- The speaker stream starts only when the circular buffer capacity is an integer multiple of the runtime DMA slot length, preventing a DMA transfer from crossing the end of the circular buffer.

Board Setting
-------------

- Connect a USB port on the PC to the PWR DEBUG port on the development board with a USB Type-C cable for programming and serial console output.
- Connect another USB port on the PC to the USB0 port on the development board for USB Audio transfers.
- Connect headphones or an active speaker to the Audio Codec :ref:`headphone <board_resource>` interface.
- Connect a microphone to the Audio Codec :ref:`microphone <board_resource>` interface.

Running the Example
-------------------

- Download and run the program. The PC automatically detects the USB Audio device and enumerates speaker and microphone audio endpoints.
- Select the speaker endpoint as the default playback device. Audio is played through the Audio Codec headphone interface.
- Select the microphone endpoint as the default input device. Audio is recorded through the Audio Codec microphone interface.
- The speaker and microphone use the same sampling clock. Select the same sample rate when both streams are active.
- Change the sample rate while the streaming interfaces are inactive, then reopen the interfaces so that the runtime DMA slot lengths are recalculated.

Speaker Feedback Status
-----------------------

During playback, the serial console periodically prints the speaker buffer level, feedback value, and USB OUT packet length range:

.. code-block:: console

   spk fb: used=12472/24576 (50%) val=1572864 hz=192000 pkt=192..192

Field descriptions:

- ``used``: Amount of data currently stored in the speaker circular buffer, in bytes.
- ``total``: Total speaker circular buffer capacity, in bytes.
- ``percent``: Speaker buffer usage. A value close to 50% indicates good synchronization between the host transmit rate and the device playback rate.
- ``val``: Raw calculated feedback value.
- ``hz``: Sample rate represented by the feedback value.
- ``pkt``: Minimum and maximum packet lengths received by the speaker USB OUT endpoint during the current statistics period, in bytes.

For example, at 192 kHz, two channels, 4-byte slots, and HS ``bInterval=1``, the nominal packet length is 192 bytes. When feedback requests a lower host transmit rate, the log can show:

.. code-block:: console

   spk fb: used=12656/24576 (51%) val=1572858 hz=191999 pkt=184..192

``pkt=184..192`` means that the host inserted an 184-byte packet, one audio frame shorter than the nominal 192-byte packet. This confirms that the explicit feedback mechanism is active. Small buffer-level oscillations around the center are expected.

Microphone Adaptive Status
--------------------------

During recording, the serial console periodically prints the microphone buffer status:

.. code-block:: console

   mic buf: used=12192/24576 (49%)

Field descriptions:

- ``used``: Amount of data currently stored in the microphone circular buffer, in bytes.
- ``total``: Total microphone circular buffer capacity, in bytes.
- ``percent``: Microphone buffer usage percentage.

The microphone adaptive mechanism operates as follows:

- When the buffer is above 62.5%, one audio frame is added to the USB IN packet to drain the buffer faster.
- When the buffer is below 37.5%, one audio frame is removed from the USB IN packet to allow the buffer to refill.
- When the buffer is between 37.5% and 62.5%, the nominal packet length is used.

A buffer level that remains in the middle region indicates good synchronization between the capture rate and host receive rate. A buffer that remains near full or empty indicates a rate mismatch.
