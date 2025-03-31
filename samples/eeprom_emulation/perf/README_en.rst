.. _eeprom_emulation_performance_test:

Eeprom emulation performance test
==================================================================

Overview
--------

The EEPROM PERF example runs performance testing on simulated EEPROM with 100 data records and 500 records.It's contain the follow:

**config time**

- format area

- write prepared data

- test config time

**flush time**

- write prepared data

- test flush time

**read time**

- write prepared data

- test read time

Board setting
-------------

- No special settings

.. note::

  - Do not write more than one erase size data once

  - Eeprom emulation needs to be set EEPROM_MAX_VAR_CNT which default count 100 in user_config.h to limit the maximum number of blocks

  - The relevant writing situation can be viewed by modifying the debug level

Running the example
-------------------

The serial port output is shown below:


.. code-block:: console

    eeprom emulation perf test
   ----------------------------------------
    1 - Test config perf
    2 - Test flush perf
    3 - Test read perf
    4 - show area base info
    Others - Show index menu

   ◆1

   ------------ flash->eeprom init ok -----------
   start address: 0x80fe0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x80ff0000
   data write addr = 0x80fe0000, info write addr = 0x80feffe4, remain flash size = 0xffe4
   valid count percent info count( 0 / 0 )
   ----------------------------------------------


   ------------ flash->eeprom init ok -----------
   start address: 0x80fe0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x80ff0000
   data write addr = 0x80fe0fa0, info write addr = 0x80fee0a4, remain flash size = 0xd104
   valid count percent info count( 100 / 500 )
   ----------------------------------------------

   eeprom config run time=(32451)us

   ◆2
   ------------ flash->eeprom init ok -----------
   start address: 0x80fe0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x80ff0000
   data write addr = 0x80fe0fa0, info write addr = 0x80fee0a4, remain flash size = 0xd104
   valid count percent info count( 100 / 500 )
   ----------------------------------------------

   eeprom flush run time=(346190)us

   ◆3
   ------------ flash->eeprom init ok -----------
   start address: 0x80ff0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x81000000
   data write addr = 0x80ff0320, info write addr = 0x80fff9a4, remain flash size = 0xf684
   valid count percent info count( 100 / 100 )
   ----------------------------------------------

   eeprom read run time=(29)us

   ◆4
   ------------ flash->eeprom init ok -----------
   start address: 0x80ff0000
   sector count: 16
   flash earse granularity: 4096
   version: 0x4553
   end address: 0x81000000
   data write addr = 0x80ff12c0, info write addr = 0x80ffda64, remain flash size = 0xc7a4
   valid count percent info count( 100 / 600 )
   ----------------------------------------------

