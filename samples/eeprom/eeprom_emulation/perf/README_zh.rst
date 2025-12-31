.. _eeprom_emulation_performance_test:

Eeprom emulation 性能测试
==========================================

概述
------

EEPROM PERF示例以100笔数据，500条记录对模拟eeprom进行性能测试，主要包括以下测试内容：

**初始化时间**

- format area

- write prepared data

- test config time

**数据整理时间**

- write prepared data

- test flush time

**读取时间**

- write prepared data

- test read time

板级设置
------------

- 无需特殊设置

.. note::

  - 一次写入的数据不要超过一个erase_size

  - 需在user_config.h文件中设定EEPROM_MAX_VAR_CNT限制写入数据笔数的最大数量，默认为100笔

  - 相关写入情况，可修改debug等级查看

运行示例
------------

当工程运行后，串口会打印以下信息：


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
