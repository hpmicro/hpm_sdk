.. _i2s_slave:

I2S Slave
==================

Overview
--------

I2S slave example shows i2s controller as slave with DMA communicate with master between board and board.
I2S slave sends a piece of audio data in a loop to master.

Workflow
---------

1. Configure system audio clock:

   - Use external MCLK, BCLK and FCLK

2. Configure I2S Slave:

   - Set FIFO threshold
   - Configure DMA request
   - Configure sample rate and bit width
   - Configure as slave mode

3. Configure DMA transfer:

   - Set source address to audio data buffer
   - Set destination address to I2S_SLAVE transmit FIFO
   - Configure source address increment mode
   - Configure destination address fixed mode
   - Enable DMA interrupt

4. Start transfer:

   - Enable I2S Slave
   - Start DMA transfer
   - Send audio data in loop

Known Issues
------------

None

Board Setting
-------------

- Connect :ref:`I2S Pins <board_resource>` between the two boards:

  - Connect Master FCLK with Slave FCLK
  - Connect Master BCLK with Slave BCLK
  - Connect Master RXD with Slave TXD
  - Connect GND together between boards

Running the Example
-------------------

When the example runs successfully:

1. Terminal output:

.. code-block:: console

   I2S Slave Demo

2. I2S Slave will continuously send audio data to I2S Master

Debugging Tips
--------------

1. Check if I2S connections are correct
2. Verify if DMA transfer is working properly
