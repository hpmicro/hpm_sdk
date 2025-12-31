.. _smix_dao:

SMIX_DAO
================

Overview
----------

This example demonstrates the functionality of using SMIX (Sound Mixer) audio mixer with DAO (Digital Audio Output) for audio playback and volume control.
SMIX can perform sample rate conversion and mixing processing on audio with different sample rates, then output differential PWM signals through DAO to drive CLASSD amplifier chips.
This example supports pause and resume functionality through GPIO button control, as well as switching SMIX mixer gain through button control.

Workflow
----------

1. Configure system audio clock (MCLK) according to the target sample rate

2. Initialize I2S peripheral and DAO peripheral:

   - Configure I2S peripheral, including audio data format (bit width, channels, sample rate)
   - Enable I2S DMA transmit request
   - Configure DAO peripheral

3. Configure GPIO button interrupt for controlling audio playback pause, resume and gain switching:

   - Set button interrupt trigger condition
   - Configure button debounce handling
   - Enable GPIO interrupt (KEY1 for pause/resume, KEY2 for gain switching)
4. Configure SMIX audio mixer:

   - Configure source channel DMA to transfer audio data from memory to SMIX source channel
   - Configure destination channel DMA to transfer SMIX processed data to I2S transmit FIFO
   - Configure burst transfer mode (mono: DMA_NUM_TRANSFER_PER_BURST_1T; stereo: DMA_NUM_TRANSFER_PER_BURST_2T)
   - Set sample rate conversion parameters (support 1x, 2x, 3x, 4x, 6x, 8x, 12x ratios)
   - Configure mixer gain and channel mask
   - Enable corresponding channels

5. Start I2S peripheral and DAO peripheral:

   - Software reset I2S peripheral and DAO peripheral
   - Start I2S peripheral and DAO peripheral

6. SMIX automatically performs sample rate conversion and mixing processing

7. SMIX DMA transfers processed audio data to I2S TX FIFO, DAO converts I2S data and outputs

8. SMIX DMA completion stops I2S peripheral, DAO peripheral and SMIX channels

9. Support pause/resume playback through KEY1 button

10. Support switching SMIX mixer gain through KEY2 button

Audio Configuration
---------------------

1. Audio format configuration

   - Support multiple input sample rates (8kHz, 16kHz, etc.)
   - Support 16-bit and 32-bit audio depth
   - Support mono (requires HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX) and stereo modes
   - Select audio data source based on compile options:

     - Mono mode: ``audio_mono.h``
     - Stereo mode: ``audio_stereo.h``

2. SMIX configuration

   - Support multiple sample rate conversion ratios: 1x, 2x, 3x, 4x, 6x, 8x, 12x

   - Support multiple gain adjustments: -12dB, -6dB, 0dB, +6dB

3. Channel configuration

   - channel_slot_mask values
     - Left channel: 0x1
     - Right channel: 0x2
     - Stereo: 0x3

   - DAO channel parameter must be configured as dual-channel (channel_slot_mask = 0x3)

   - To play a mono audio source with DAO, configure I2S channel_slot_mask (requires HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX):

     - 0x1 plays on DAO left channel; 0x2 plays on DAO right channel
     - If HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX is not supported, duplicate mono data to stereo and play as stereo

   - To play a stereo audio source with DAO, set I2S channel_slot_mask = 0x3 so both left and right output

Hardware Settings
--------------------

- Connect a speaker to the development board's :ref:`DAO interface <board_resource>` for audio playback

- Use the development board's buttons :ref:`button interface <board_resource>` for controlling playback:

  - KEY1: Control playback pause/resume
  - KEY2: Switch SMIX mixer gain(-12dB - 0dB), the initial gain is -12dB

Known Issues
-------------

- Some audio may produce noise when played through DAO

- On-board buttons may trigger multiple GPIO interrupts due to mechanical bounce

Running the Example
---------------------

When the project runs correctly, you should observe:

1. Terminal output:

   .. code-block:: console

      Sound mixer change DAO volume example
      Press Key1 will pause/resume audio play, Press Key2 will change audio play gain.

      -- Select audio play action --
       1 - audio play

2. After selecting option 1:

   - Terminal displays "mixer play one sound"
   - Shows input and output sample rate information
   - Audio device starts playing sample audio(The initial gain is -12dB)

3. Button control during playback:

   - Press KEY1 button to pause playback, press KEY1 again to resume
   - Press KEY2 button to switch SMIX mixer gain between -12dB and 0dB
   - Terminal will display corresponding control information:

     - "dao_i2s pause" - pause playback
     - "dao_i2s resume" - resume playback
     - "set SMIX gain to 0dB" - set gain to 0dB
     - "set SMIX gain to -12dB" - set gain to -12dB

