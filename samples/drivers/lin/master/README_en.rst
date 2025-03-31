.. _lin_master:

lin_master
====================

Overview
--------

The lin_master sample project shows that LIN sends/receives data at master mode.

Board Setting
-------------

Requires a LIN transceiver and USB_LIN debugger
Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
connect LIN transceiver mcu signal to LIN's TX and RX on board, connect LIN transceiver LIN signal to USB_LIN debugger.

Running the example
-------------------

Configure usb_lin debugger:
- Configure com and baudrate, then click `Set to Debugger` option

  .. image:: ../doc/lin_debugger_configuration.png
     :alt: lin_debugger_configuration

- Set ID, data and checksum mode, tick `enable option`， then click `Configure to Debugger` option：

  .. image:: doc/lin_debugger_slave_sent_config.png
     :alt: lin_debugger_slave_sent

- Running the example, check the result in debugger window

  .. image:: doc/lin_debugger_slave_result.png
     :alt: lin_debugger_slave_result

When the example runs successfully, the log would be seen on the terminal like:

.. code-block:: console

   LIN master example
   LIN master sent wakeup signal
   ID: 31, sent 8 bytes:
   0 1 2 3 4 5 6 7
   ID: 30, receive 8 bytes
   7 6 5 4 3 2 1 0

