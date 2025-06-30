.. _i2s_emulation:

I2S EMULATION
==========================

Overview
--------

- This example project demonstrates the functionality of using SPI to emulate an I2S master for receiving audio decoding module recording data and playing back the recorded data.

- The evaluation board must be connected to an audio decoding module. This example uses the WM8978 codec.

Tips
----

- For detailed documentation about I2S over SPI, please refer to :ref:`i2s_over_spi <i2s_over_spi>`

Notice
------

- Due to the requirement for long data transfers in this example, it is currently supported **only on the hpm5300 series**.

dependent components
--------------------

- dma_mgr

- spi

- i2s_over_spi

- codec(wm8978)

Board Setting
-------------

- Use the onboard microphone of the audio module for recording.

- Connect a 3.5mm headphone to the audio module's output headphone jack.

- The following components are required: GPTMR output and SPI pins (Please refer to   :ref:`Pin Description <board_resource>` )


- Pin connections:

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

- When the project runs correctly, the serial terminal will display the following menu:


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


- Enter the character `'3'` in the terminal to start recording. The system will automatically stop and save the recording when the duration exceeds the specified limit.


.. code-block:: console

   record_start enter....
   recording size overflow, stopping and exiting now...


- Enter the character `'1'` in the terminal to start playback.


.. code-block:: console

   play start enter....
   play end, now exit....


- Enter the character `'4'` in the terminal to stop recording.


.. code-block:: console

   record stop finish....


- Enter the character `'2'` in the terminal to stop playback.


.. code-block:: console

   play stop finish....

