.. _i2s_master:

I2S Master
====================

Overview
--------

The I2S master example shows i2s controller as i2s master with DMA to communicate with i2s slave between board and board.
I2S master reads audio data from I2S slave, and plays through DAO.

Workflow
---------

1. Configure system audio clock

2. Configure I2S Master:

   - Enable MCLK output
   - Set FIFO threshold
   - Configure DMA request
   - Configure sampling rate and bit width

3. Configure I2S_DAO and DAO:

   - Configure sampling rate and bit width

4. Configure DMA transfer:

   - Set source address to I2S_MASTER receive FIFO
   - Set destination address to I2S_DAO transmit FIFO
   - Configure fixed address mode
   - Enable DMA interrupt

5. Start transfer:

   - Enable I2S Master
   - Enable I2S_DAO and DAO
   - Start DMA transfer

Known Issues
------------

When using DAO to play some audio, it may generate noise.

Hardware Setup
--------------

- Connect :ref:`I2S Pins <board_resource>` between the two boards:

  - Connect Master FCLK with Slave FCLK
  - Connect Master BCLK with Slave BCLK
  - Connect Master RXD with Slave TXD
  - Connect GND together between boards

- Connect a speaker to the DAO interface on the development board

Running the Example
-------------------

When the example runs successfully:

1. Terminal output:

.. code-block:: console

   I2S Master example

2. You can hear a piece of audio about a brief introduction of HPMicro

Debugging Tips
---------------

1. Check if I2S connections are correct
2. Verify if DMA transfer is working properly
