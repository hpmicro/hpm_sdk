.. _modbus_rtu_master:

modbus rtu master
==================================

Overview
--------

- This project demonstrates the modbus rtu master function

- In this example project, one board serves as rtu master and the other board serves as rtu slave. The master sends a read holding register and waits for the slave to respond to the request data.

Note
----

- When using uart for modbus communication and communicating with an rtu slave, you must reset the slave first and then the host.

Board Setting
-------------

- Please refer to the description in the specific development board {ref}` pin description <board_resource>` section.

- Connect the modbus uart pins of the two boards.

- The two boards must be connected to the GND of both sides to share the ground to ensure signal transmission.

Running the example
-------------------

- Run slave first, then master

- When the project runs correctly, the serial terminal will output the following information:


.. code-block:: console

   modbus rtu master example
   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

