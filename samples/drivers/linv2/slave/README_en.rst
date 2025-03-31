.. _linv2_slave:

linv2_slave
======================

Overview
--------

The linv2_slave sample project shows that LINV2 sends/receives data at slave mode.

Board Setting
-------------

Requires a USB_LIN debugger
Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
connect LIN signal on board to USB_LIN debugger.

Running the example
-------------------

configure usb_lin debugger:
- configure com and baudrate, then click `Set to Debugger` option

  .. image:: ../../lin/doc/lin_debugger_configuration.png
     :alt: lin_debugger_configuration

- Configure sent data, including master sent, master receive and sync break, the sync break could wake up slave from sleep mode：

  .. image:: ../../lin/slave/doc/lin_debugger_master_sent_config.png
     :alt: lin_debugger_master_sent

- Running the example, check the result in debugger window

  .. image:: ../../lin/slave/doc/lin_debugger_master_result.png
     :alt: lin_debugger_master_result

When the example runs successfully, the log would be seen on the terminal like:

.. code-block:: console

   LIN slave example
   LIN bus timeout, will enter sleep mode
   LIN wakeup
   ID: 31, receive 8 bytes
   0 1 2 3 4 5 6 7
   ID: 30, sent 8 bytes
   7 6 5 4 3 2 1 0

