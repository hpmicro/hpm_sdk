.. _pmbus_slave:

Pmbus slave
======================

Overview
--------

- The PMbus Slave example project demonstrates the I2C interface operating in slave mode as a PMbus device for receiving and transmitting data. It requires registering the PMbus command transaction handlers, and the PMbus slave protocol stack will print the received or transmitted data along with command-related messages via the callback function.

Tips
----

- For detailed documentation about PMbus components, please refer to :ref:`pmbus <pmbus>`

Board Setting
-------------

connect  :ref:`I2C Pins <board_resource>`  on the two boards。

Running the example
-------------------

- Run the slave firmware first, followed by the master.

- When the project runs correctly, the serial terminal will display output similar to the following:


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
