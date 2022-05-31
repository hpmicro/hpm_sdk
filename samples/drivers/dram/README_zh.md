# DRAM example
## 概述

DRAM示例工程展示了读写内部或外部DRAM存储器的性能。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
dram example start @ 166666666
comparison test: from 0x40000000 to 0x42000000
d-cached enabled and flushed
**  comparison succeeded
comparison test: from 0x40000000 to 0x42000000
d-cached disabled
**  comparison succeeded
Testing XRAM, cached enabled
d-cached enabled
read performance @0x1088000, 32768KB
read throughput: 524644.76 KB/s
write performance @0x1088000, 32768KB
write throughput: 403097.53 KB/s
Testing SDRAM, cached enabled
d-cached enabled
read performance @0x40000000, 32768KB
read throughput: 174914.49 KB/s
write performance @0x40000000, 32768KB
write throughput: 145822.72 KB/s
Testing XRAM, cached disabled
d-cached disabled
read performance @0x1088000, 32768KB
read throughput: 86795.20 KB/s
write performance @0x1088000, 32768KB
write throughput: 106643.10 KB/s
Testing SDRAM, cached disabled
d-cached disabled
read performance @0x40000000, 32768KB
read throughput: 25055.62 KB/s
write performance @0x40000000, 32768KB
write throughput: 45559.44 KB/s
dram example end

```
