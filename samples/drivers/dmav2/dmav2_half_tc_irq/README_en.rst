.. _dmav2_half_transfer_complete_interrupt:

DMAV2 Half Transfer Complete Interrupt
============================================================================

Overview
--------

- DMAV2 has added half transfer complete interrupt. This example project shows the use of circularly transfer to receive UART DATA. DMAV2 generates interrupts when receiving half data of the buffer and full data of the buffer. This feature can achieve a function similar to ping-pong buffer for data collection, meanwhile DMA not stop.

Board Setting
-------------

- Use the serial port debugging assistant to send and receive data

- The serial port is set to ``115200bps`` , ``1 stop bit`` , ``no parity bit``

Running the example
-------------------

- Use the serial port debugging assistant to send 32 bytes data, and the development board will send back the received 32 bytes data, as shown in the following example


.. code-block:: text

   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF

