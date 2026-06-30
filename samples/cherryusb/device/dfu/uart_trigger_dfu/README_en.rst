.. _uart_trigger_dfu:

UART Trigger DFU
======================

Overview
--------

uart_trigger_dfu example project demonstrates a UART trigger DFU functionality. This application monitors the console UART for a specific trigger sequence. When the user types "dfu" on the console UART, the device will reboot into DFU mode, allowing firmware updates through USB or other DFU interfaces. This example requires DFU bootloader, ensure the device has been correctly programmed with the DFU bootloader.

Board Setting
-------------

- Connect console UART to PC

Running the example
-------------------

When the project runs correctly, the serial terminal will output:

.. code-block:: console

   DFU UART trigger demo
   Type 'dfu' on console UART to reboot into DFU mode
   dfu trigger received, reboot to DFU bootloader...

Type "dfu" on the console UART to trigger a reboot into DFU mode.
