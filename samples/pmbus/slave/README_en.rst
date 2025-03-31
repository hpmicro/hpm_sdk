.. _pmbus_slave:

Pmbus slave
======================

Overview
--------

This example demonstrates I2C operating in slave mode as a PMbus device for data reception and transmission. You need to register handlers for PMbus command transactions. The PMbus slave protocol stack uses completion callback functions to print received or transmitted data and command-related messages.

Board Setting
-------------

connect  :ref:`I2C Pins <board_resource>`  on the two boards。

Running the example
-------------------

- Run slave first, and then run master.

- When the example runs successfully, the log would be seen on the terminal like:


.. code-block:: console

   PMbus slave test
   pmbus(cmd:0x05) slave command transaction install success
   pmbus(cmd:0x06) slave command transaction install success
   pmbus(cmd:0x00) slave command transaction install success
   pmbus(cmd:0x03) slave command transaction install success
   pmbus(cmd:0x22) slave command transaction install success
   cb:cmd: 0x05, read:true, pec_check:true, len:128, data_addr:0x0120a054
   cb:cmd: 0x00, read:true, pec_check:true, len:1, data_addr:0x0120a054
   cb:cmd: 0x03, read:true, pec_check:true, len:0, data_addr:0x0120a054
   cb:cmd: 0x22, read:true, pec_check:true, len:2, data_addr:0x0120a054
   cb:cmd: 0x00, read:false, pec_check:true, len:1, data_addr:0x01209fd4
   cb:cmd: 0x06, read:false, pec_check:true, len:128, data_addr:0x01209fd4
