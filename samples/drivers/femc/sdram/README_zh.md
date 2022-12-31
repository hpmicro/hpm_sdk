# SDRAM example
## 概述

SDRAM示例工程展示了读写内部或外部SDRAM存储器的性能。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
sdram example start @ 166666666
Dcache Enabled
Comparison test
comparison test: from 0x40000000 to 0x42000000
d-cached enabled and flushed
**  comparison succeeded
Testing SDRAM
d-cached enabled
read performance @0x40000000, 32768KB
read throughput: 117288.25 KB/s
write performance @0x40000000, 32768KB
write throughput: 103431.06 KB/s
Dcache Disabled
Comparison test
comparison test: from 0x40000000 to 0x42000000
d-cached disabled
**  comparison succeeded
Testing SDRAM
d-cached disabled
read performance @0x40000000, 32768KB
read throughput: 20472.71 KB/s
write performance @0x40000000, 32768KB
write throughput: 33216.41 KB/s
sdram example end
```
