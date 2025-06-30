.. _pmbus_master:

Pmbus master
========================

Overview
--------

- The PMbus Master example project demonstrates the I2C interface operating in master mode to function as a PMbus controller for transmitting and receiving data. It repeatedly sends different PMbus commands (byte, word, and block instructions) and prints the length of the read/written data or error codes.

Tips
----

- For detailed documentation about PMbus components, please refer to :ref:`pmbus <pmbus>`

Board Setting
-------------

- Connect the :ref:`I2C pins <board_resource>` between two boards.

Running the example
-------------------

- Run the slave firmware first, followed by the master.

- When the project runs correctly, the serial terminal will display output similar to the following


.. code-block:: console

   PMbus master test
   pmbus master write test begin...
   pmbus master write test end...

   pmbus master read test begin...
   pmbus(cmd:0x00) master read len:1
   pmbus(cmd:0x06) master read len:128
   pmbus(cmd:0x22) master read len:2
   pmbus master read test end...
