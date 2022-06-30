# DRAM example
## Overview

DMA example project shows performance of reading and writing data in internal or external DRAM memory.

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
dram example start @ 166666666
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
dram example end

```