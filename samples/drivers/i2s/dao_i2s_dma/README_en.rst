.. _dao_i2s_dma:

DAO_I2S_DMA
==============

Overview
----------

This example demonstrates the functionality of using DAO (Digital Audio Output) + I2S peripheral with DMA to transfer audio data for playback.
DAO is a digital audio output module that can directly output differential PWM signals to drive CLASSD amplifier chips for audio playback.
This example also supports pause and resume functionality through GPIO button control.

Workflow
----------

1. Configure system audio clock (MCLK) according to the audio sampling rate

2. Initialize I2S peripheral and DAO peripheral:

   - Configure I2S peripheral, including audio data format (bit width, channels, sampling rate)
   - Enable I2S DMA transmit request
   - Configure DAO peripheral

3. Configure GPIO button interrupt for controlling audio playback pause and resume:

   - Set button interrupt trigger condition
   - Configure button debounce handling
   - Enable GPIO interrupt

4. Configure DMA to transfer audio data:

   - Configure DMAMUX, select I2S as DMA request source
   - Enable DMA interrupt
   - Set source address to audio data storage location
   - Set destination address to I2S transmit FIFO
   - Configure transfer size and width
   - Configure burst transfer mode (mono: DMA_NUM_TRANSFER_PER_BURST_1T; stereo: DMA_NUM_TRANSFER_PER_BURST_2T)

5. Start I2S peripheral and DAO peripheral:

   - Software reset I2S peripheral and DAO peripheral
   - Start I2S peripheral and DAO peripheral

6. DMA automatically transfers audio data to I2S TX FIFO, DAO converts I2S data and outputs

7. Stop I2S peripheral and DAO peripheral after DMA completion

8. Repeat steps 4-7 to achieve audio loop playback

9. Support pause/resume playback through button during audio playback

Audio Configuration
----------------------

1. Audio format configuration

   - Support multiple audio sampling rates
   - Support 16-bit and 32-bit audio depth
   - Support mono (requires HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX) and stereo modes
   - Select audio data source based on compile options:

     - Mono mode: ``audio_mono.h``
     - Stereo mode: ``audio_stereo.h``

2. Channel configuration

   - channel_slot_mask values

     - Left channel: 0x1
     - Right channel: 0x2
     - Stereo: 0x3

   - DAO channel parameter must be configured as dual-channel (channel_slot_mask = 0x3)

   - To play mono audio source with DAO, use I2S channel_slot_mask when supported

     - Requires HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX
     - Set I2S channel_slot_mask to mono: 0x1 plays on DAO left, 0x2 plays on DAO right
     - If HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX is not supported, duplicate mono data to form stereo and play as stereo

   - To play stereo audio source with DAO, set I2S channel_slot_mask = 0x3 so both left and right output

Hardware Settings
-------------------

- Connect a speaker to the development board's :ref:`DAO interface <board_resource>` for audio playback

- Use the development board's button :ref:`button interface <board_resource>` for controlling playback pause/resume

Known Issues
---------------

- Some audio may produce noise when played through DAO

- On-board buttons may trigger multiple GPIO interrupts due to mechanical bounce

Running the Example
----------------------

When the project runs correctly, you should observe:

1. Terminal output:

   .. code-block:: console

      DAO_I2S with DMA example
      DAO_I2S with DMA play test start, press button can pause/resume audio play.
      DAO_I2S with DMA play one cycle finished
      DAO_I2S with DMA play one cycle finished
      DAO_I2S with DMA play one cycle finished
      DAO_I2S with DMA play one cycle finished
      Test DAO_I2S DMA play success

2. The audio device will play the sample audio in a loop (4 times total)

3. Press the button during audio playback to pause, press again to resume

4. Terminal will display corresponding control information:

   - "dao_i2s pause" - pause playback
   - "dao_i2s resume" - resume playback

