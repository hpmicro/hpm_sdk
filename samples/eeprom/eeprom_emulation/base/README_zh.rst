.. _eeprom_emulation_base_test:

Eeprom emulation 功能测试
==========================================

概述
------

EEPROM BASE示例对模拟eeprom的write、read、flush接口进行测试，主要包括以下测试内容：


**写入**

- 写入4笔数据

- 查看返回值是否正确


**读取**

- 读出写入的4笔数据

- 查看返回值是否正确

- 与写入数据进行比较


**数据更新**

- 更新前两笔的数据

- 查看更新情况


**数据整理**

- 整理所设置区域的数据，保留最新数据


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

