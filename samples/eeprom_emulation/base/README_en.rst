.. _eeprom_emulation_base_test:

Eeprom emulation base test
====================================================

Overview
--------

EEPROM BASE example tests the write, read, and flush interfaces. It's contain the follow:

**write**

- write data

- check return value

**read**

- read written data

- check return value

- compare data

**update data**

- update written data

- view updates

**flush data**

- flush the data of the set area, keep the latest data

Board setting
-------------

- No special settings

.. note::

  - Do not write more than one erase size data once

  - EEPROM_MAX_VAR_CNT which default count 100 needs to be set in user_config.h to limit the maximum number of blocks

  - The relevant writing situation can be viewed by modifying the debug level

Running the example
-------------------

The serial port output is shown below:


.. code-block:: console

   ------------ flash->eeprom init ok -----------
   start address: 0x80fd0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x80fe0000
   data write addr = 0x80fd0021, info write addr = 0x80fdffa4, remain flash size = 0xff83
   valid count percent info count( 0 / 0 )
   ----------------------------------------------

   ----------------------------------------------

    eeprom emulation demo
   ----------------------------------------
    1 - Test eeprom write
    2 - Test eeprom read
    3 - Test eeprom update data
    4 - Test eeprom flush whole area
    5 - show area base info
    Others - Show index menu

   ◆1

   ◆2
   var1 = abcdef
   var2 = 1234
   var3 = hello,world
   var4 = eeprom_demo

   ◆3

   ◆5
   ------------ flash->eeprom init ok -----------
   start address: 0x80fd0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x80fe0000
   data write addr = 0x80fd0072, info write addr = 0x80fdff04, remain flash size = 0xfe92
   valid count percent info count( 4 / 10 )
   ----------------------------------------------

   ◆2
   var1 = qwe
   var2 = 5678
   var3 = hello,world
   var4 = eeprom_demo

   ◆4
   ------------ flash->eeprom init ok -----------
   start address: 0x80fc0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x80fd0000
   data write addr = 0x80fc0021, info write addr = 0x80fcffa4, remain flash size = 0xff83
   valid count percent info count( 4 / 4 )
   ----------------------------------------------

