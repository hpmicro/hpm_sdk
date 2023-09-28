# Eeprom emulation 性能测试

## 概述

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

## 板级设置

- 无需特殊设置

## 说明
- 一次写入的数据不要超过一个erase_size
- 需在user_config.h文件中设定EEPROM_MAX_VAR_CNT限制写入数据笔数的最大数量，默认为100笔

## 运行示例

当工程运行后，串口会打印以下信息：

```console

 eeprom emulation perf test
----------------------------------------
 1 - Test config perf
 2 - Test flush perf
 3 - Test read perf
 4 - show area base info
 Others - Show index menu

收←◆1

check version failed, begin earse all sector, it will take some time

------------ flash->eeprom init ok -----------

start address: 0x80080000
sector count: 128
flash earse granularity: 4096
version: 0x4553
end address: 0x80100000
data write addr = 0x80080000, info write addr = 0x800fffe0, remain flash size = 0x7ffe0

valid count percent info count( 0 / 0 )

----------------------------------------------

收←◆2
------------ flash->eeprom init ok -----------

start address: 0x80080000
sector count: 128
flash earse granularity: 4096
version: 0x4553
end address: 0x80100000
data write addr = 0x80080fa0, info write addr = 0x800fe0a0, remain flash size = 0x7d100

valid count percent info count( 100 / 500 )

----------------------------------------------

eeprom flush run time=(6103720)us

收←◆3

eeprom read run time=(54)us
```