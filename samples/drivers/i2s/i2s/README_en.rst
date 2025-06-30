.. _i2s_dao_pdm:

I2S_DAO_PDM Audio Example
============================

Overview
------------

This example demonstrates how to use the I2S interface for audio recording and playback. The sample includes PDM (Pulse Density Modulation) recording and DAO (Digital Audio Output) playback functionality.

The demonstration includes the following features:

- DAO audio playback
- PDM recording and DAO playback

Workflow
--------

DAO Playback Workflow:

1. Configure system audio clock:
   - Default configuration is 24.576MHz, suitable for 48KHz sampling rate
   - System clock can be adjusted based on actual sampling rate requirements
2. Configure I2S1 peripheral for DAO playback:
   - Set sampling rate (e.g., 48KHz)
   - Configure stereo mode
3. Configure DAO:
4. Start audio playback:
   - Enable I2S1 peripheral
   - Enable DAO
5. Send audio data to DAO through I2S1's TX0 FIFO:
   - Check FIFO status and fill audio data accordingly
6. Disable DAO and I2S1 peripheral when playback is complete

PDM Recording Workflow:

1. Configure system audio clock:
   - Default configuration is 24.576MHz, suitable for 48KHz sampling rate
   - System clock can be adjusted based on actual sampling rate requirements
2. Configure I2S0 peripheral for PDM recording:
   - Set sampling rate
   - Configure channels (via channel_slot_mask)
   - Set 32-bit data width
3. Configure PDM:
   - Set clock division
   - Configure CIC decimation rate
   - Select working channels
4. Start recording:
   - Enable I2S0 peripheral
   - Enable PDM
5. Receive PDM recording data through I2S0's RX0 FIFO:
   - Check FIFO status and read data accordingly
6. Disable PDM and I2S0 peripheral when recording is complete

Technical Details
-----------------

- DAO Playback Features:

  - Sampling Rate: DAO works optimally at 48KHz
  - Channels: Fixed as stereo, single-channel audio requires data source processing
  - Sample Width: Configurable
  - DAO works in coordination with I2S1, sending audio data through I2S1's TX0 FIFO to DAO

- PDM Recording Features:

  - Sampling Rate: Configurable
  - Channels: Supports up to 8 channels (4 stereo pairs)
  - Sample Width: 24-bit PCM + 4-bit index, using 32-bit data width configuration
  - PDM works in coordination with I2S0, receiving audio data through I2S0's RX0 FIFO

- Usage Notes:

  - I2S register data interface places valid data in the high bits of 32-bit registers. When using DAO to play 16-bit audio,
    the 16-bit audio data needs to be written to the high 16 bits of the 32-bit register.
    When recording with PDM, if you want to preserve the recording in 16-bit format, you need to read the high 16 bits from the 32-bit register.
  - When using PDM recording, the sampling rates of PDM and I2S0 must be synchronized and kept consistent.
    PDM sampling rate can be adjusted through clock division and CIC decimation rate.
  - PDM channel configuration: channels are configured through the channel_slot_mask parameter, and channels are arranged in the order of D0L\D1L\D2L\D3L\D0R\D1R\D2R\D3R
    For example: channel_slot_mask = 0x11, which means using the left and right channels of the D0 data line
  - The default system audio clock is 24.576MHz, suitable for sampling rates like 48KHz, 16KHz, etc.
    For other sampling rates like 44.1KHz, system clock configuration needs to be modified.

Known Issues
------------

Noise may occur when playing certain audio through DAO.

Hardware Requirements
---------------------

- Connect speakers to the DAO interface on the board

Running the Example
-------------------

After power-up, the program will execute the following tests in sequence:

1. DAO WAV File Playback:

   - Plays a preset WAV audio with 8KHz sampling rate, mono channel, 16-bit depth

2. DAO Sine Wave Playback:

   - Plays a 1KHz sine wave, 48KHz sampling rate, mono channel, 32-bit depth, Repeats 200 times

3. PDM Recording and DAO Playback:

   - First performs PDM recording
   - Then plays back the recorded audio through DAO

The serial console will display the progress of each test:

.. code-block:: console

   DAO and PDM with I2S example

   1. Testing DAO wav playback

   2. Testing DAO sine wave playback

   3. Testing PDM record and DAO playback
   Please enter any character to start recording
   Recording finish
   Please enter any character to start playing
   Playing finish

