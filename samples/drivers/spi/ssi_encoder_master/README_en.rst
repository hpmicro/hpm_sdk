.. _spi_read_ssi_encoder:

SPI READ SSI ENCODER
==========================================================================

Overview
--------

This example demonstrates reading an SSI encoder via SPI and printing the related data.

Hardware Setup
--------------

This example uses another development board running the `hpm_sdk/samples/drivers/sei/slave/ssi` sample to simulate an SSI Encoder.

Connect the board's :ref:`SPI pins <board_resource>` SCLK and MISO pins to the SEI's CLOCK and DATA signals respectively through a TTL to RS485 module.

Running the example
----------------------

When the project runs correctly, the serial terminal will output the following information:

.. code-block:: console

   SPI Master Read SSI Encoder Example
   SSI Encoder Data: [0xa5afa6]
   SSI Encoder Data: [0xa5afa7]
   SSI Encoder Data: [0xa5afa8]
   SSI Encoder Data: [0xa5afa9]
   SSI Encoder Data: [0xa5afaa]
   SSI Encoder Data: [0xa5afab]
   SSI Encoder Data: [0xa5afac]
   SSI Encoder Data: [0xa5afad]
   SSI Encoder Data: [0xa5afae]
   SSI Encoder Data: [0xa5afaf]
   SSI Encoder Data: [0xa5afb0]
   SSI Encoder Data: [0xa5afb1]
   SSI Encoder Data: [0xa5afb2]
   SSI Encoder Data: [0xa5afb3]
   SSI Encoder Data: [0xa5afb4]
   SSI Encoder Data: [0xa5afb5]
   SSI Encoder Data: [0xa5afb6]
   SSI Encoder Data: [0xa5afb7]
   SSI Encoder Data: [0xa5afb8]
