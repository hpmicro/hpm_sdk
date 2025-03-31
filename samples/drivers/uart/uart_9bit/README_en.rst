.. _uart_9bit:

UART_9bit
==================

Overview
--------

This sample show UART commucate with address match on 9bit mode. The UART master can send data to slave with a specific address, while the slave can only receive data when addressed.
In this example, connect the TX and RX pins of the UART, initialize the UART and set the address. Then UART sends address and data. When the sent address matches the set address, UART will receive the sent data, otherwise it cannot receive the data.

Board Setting
-------------

- connect uart tx to rx pin(Please refer to   :ref:`Pin Description <board_resource>` )

Running the example
-------------------

When the project runs correctly, the serial port terminal will output the information like:

.. code-block:: console

   uart 9bit address match transmit example
   uart send address: 0x76 and data:
   uart not receive any data
   uart send address: 0x77 and data:
   uart receive data:
   0x77 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17
