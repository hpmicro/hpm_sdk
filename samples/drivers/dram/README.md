# DRAM example
## Overview

DMA example project shows performance of reading and writing data in internal or external DRAM memory.

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
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