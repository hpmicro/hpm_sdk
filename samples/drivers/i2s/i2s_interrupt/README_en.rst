.. _i2s_interrupt:

I2S Interrupt
==========================

Overview
---------

This example demonstrates how to use I2S interrupt mode to transfer audio data to the onboard audio codec chip for playback.

Workflow
---------

1. Configure system audio clock (MCLK) according to the audio sampling rate
2. Configure I2S peripheral:
   - Enable MCLK output to Codec
   - Set audio data format (bit width, number of channels)
   - Configure TX FIFO interrupt threshold
   - Enable transmit FIFO interrupt
3. Configure Codec:
   - Set audio data format and sampling rate
   - Configure Codec to playback mode
   - Configure audio output path (e.g., headphone output)
4. Enable I2S peripheral to start audio data transfer
5. When I2S generates TX FIFO interrupt, in the interrupt handler:
   - Read audio data
   - Align data according to audio depth
   - Fill data into I2S TX FIFO
6. Disable I2S interrupt and I2S peripheral when playback is complete

Project Configuration
----------------------

1. Audio Codec Configuration
   - In the `CMakeLists.txt` file, set the matching audio codec type according to the board schematic
   - For example: "set(CONFIG_CODEC "sgtl5000")"

Hardware Setup
---------------

- Connect 3.5mm headphones to the audio codec's :ref:`headphone <board_resource>` interface

Known Issues
-------------

- On some development boards, there might be crosstalk when playing audio through the codec's headphone interface. For example, when playing mono audio through the left channel, a weak sound might be heard in the right channel headphone.

Important Notes
----------------

- Audio codec clock and configuration sequence:

  - Most Audio codecs require MCLK from the I2S interface as the master clock source
  - MCLK must be configured and enabled first, then the Codec can be configured through I2C interface after it's working

Running Results
----------------

When the project runs correctly, you can observe:

1. Terminal output:

.. code-block:: console

   I2S Interrupt example
   I2C bus is ready
   I2S interrupt play finished

2. The headphones will play the sample audio in a loop

Debugging Tips
---------------

1. Verify that the audio codec model is correctly configured
2. Check if the I2C bus is properly initialized and the codec configuration is completed correctly
3. Monitor if interrupts are triggering properly
4. Use an oscilloscope to check if I2S signal timing is correct

