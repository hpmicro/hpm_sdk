.. _i2s_dma:

I2S DMA
==============

Overview
----------

This example demonstrates the functionality of using I2S peripheral with DMA to transfer audio data and playback through the onboard codec.

Workflow
--------

1. Configure system audio clock (MCLK) according to the audio sampling rate
2. Configure I2S peripheral:
   - Enable MCLK output to Codec
   - Set audio data format (bit width, number of channels)
   - Configure DMA transfer mode
   - Enable DMA request
3. Configure Codec:
   - Set audio data format and sampling rate
   - Configure Codec to playback mode
   - Configure audio output path (e.g., headphone output)
4. Configure DMA:
   - Set source address to audio data buffer
   - Set destination address to I2S TX FIFO
   - Configure transfer size and width
5. Start I2S peripheral and DMA transfer:
   - Enable I2S peripheral
   - Start DMA transfer
6. DMA automatically transfers audio data to I2S TX FIFO
7. Disable DMA transfer and I2S peripheral when playback is complete

Project Configuration
----------------------

- In ``CMakeLists.txt``, set the appropriate audio codec type according to your board schematic:

  - WM8960: ``set(CONFIG_CODEC "wm8960")``
  - SGTL5000: ``set(CONFIG_CODEC "sgtl5000")``

Hardware Settings
-----------------

- Connect a 3.5mm headphone to the :ref:`headphone jack <board_resource>` of the audio codec chip

Known Issues
-------------

- On some development boards, there might be audio crosstalk when using the codec's headphone interface. For example, when playing mono audio through the left channel, a faint sound might be heard in the right channel headphone.

Important Notes
---------------

- Audio codec clock and configuration sequence:

  - Most Audio codecs require MCLK from the I2S interface as their master clock source
  - MCLK must be configured and enabled first before the codec's working parameters can be configured through I2C interface

Running the Example
--------------------

When the project runs correctly, you should observe:

1. Terminal output:

   .. code-block:: console

      I2S DMA example
      I2C bus is ready
      i2s dma play finished

2. The headphone will play the sample audio in a loop

