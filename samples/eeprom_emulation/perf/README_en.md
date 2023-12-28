# Eeprom emulation performance test

## Overview

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

## Board setting

- No special settings

## Notice

- Do not write more than one erase size data once
- Eeprom emulation needs to be set EEPROM_MAX_VAR_CNT which default count 100 in user_config.h to limit the maximum number of blocks

## Running the example

The serial port output is shown below:

```console

 eeprom emulation perf test
----------------------------------------
 1 - Test config perf
 2 - Test flush perf
 3 - Test read perf
 4 - show area base info
 Others - Show index menu

◆1

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

◆2
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

◆3

eeprom read run time=(54)us
```
