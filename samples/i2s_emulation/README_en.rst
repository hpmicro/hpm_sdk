.. _i2s_emulation:

I2S EMULATION
==========================

Overview
--------

- This sample show use SPI transmit and receive audio data to audio codec

- The evaluation board needs to be connected to the audio decoding module. This example uses wm8978

Notice
------

- Since the SPI used in this example requires a large transmission length, it currently only supports the hpm5300 series.

dependent components
--------------------

- dma_mgr

- spi

- i2s_over_spi

- codec(wm8978)

Board Setting
-------------

- Recording via the onboard mic of the audio module

- Connecting 3.5mm headphones to the headphone output port of the audio module

- Need to use GPTMR output and SPI pins，pins(Please refer to   :ref:`Pin Description <board_resource>` )

- 5V          <--> 5V(WM8978)

- 3.3V        <--> 3.3V(WM8978)

- GND         <--> GND(WM8978)

- MCLK(GPTMR) <--> MCLK(WM8978)

- LRCK(GPTMR) <--> (FSA)LRC(WM8978)

- BLCK(GPTMR) <--> SCLK(SPI) <--> SCK(WM8978)

- MISO(SPI)   <--> (DAT)DACDATA(WM8978)

- MOSI(SPI)   <--> (SDB)ADCDATA(WM8978)

- (CS)GPIO    <--> CS(SPI)

- I2C_SDA     <--> I2C_SDA(WM8978)

- I2C_SCL     <--> I2C_SCL(WM8978)

Run steps
---------

- When the project runs correctly, the serial port terminal will output the information like


.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   i2s_emulation demo                        *
   *                                                             *
   *        1 - start play                                       *
   *        2 - stop play                                        *
   *        3 - start record                                     *
   *        4 - stop record                                      *
   *-------------------------------------------------------------*


- Enter the '3' character into the terminal to start recording. If the recording duration is longer than the specified duration, it will automatically exit and save.


.. code-block:: console

   record_start enter....
   record size overflow, now exit..


- Enter the '1' character in the terminal to start recording playback


.. code-block:: console

   play start enter....
   play end, now exit....


- Enter the '4' character in the terminal to stop recording.


.. code-block:: console

   record stop finish....


- Enter the '2' character in the terminal to stop playing


.. code-block:: console

   play stop finish....

