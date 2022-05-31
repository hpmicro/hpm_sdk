# DMA example
## Overview

DMA example project shows transfer data with unchined and chained way. This example checks if the data transferred by DMA is correct.

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
dma example start
unchained transfer
write testing
dma transferring data from 0x10c2000 to 0x40000000, burst size: 8192 bytes
rw throughput: 182498.65 KB/s
compare data between source address 0x10c2000 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3000 to 0x40000000, burst size: 4096 bytes
rw throughput: 93331.31 KB/s
compare data between source address 0x10c3000 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3800 to 0x40000000, burst size: 2048 bytes
rw throughput: 62665.74 KB/s
compare data between source address 0x10c3800 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3c00 to 0x40000000, burst size: 1024 bytes
rw throughput: 47139.70 KB/s
compare data between source address 0x10c3c00 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3e00 to 0x40000000, burst size: 512 bytes
rw throughput: 37761.82 KB/s
compare data between source address 0x10c3e00 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3f00 to 0x40000000, burst size: 256 bytes
rw throughput: 31453.17 KB/s
compare data between source address 0x10c3f00 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3f80 to 0x40000000, burst size: 128 bytes
rw throughput: 26881.58 KB/s
compare data between source address 0x10c3f80 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3fc0 to 0x40000000, burst size: 64 bytes
rw throughput: 22639.34 KB/s
compare data between source address 0x10c3fc0 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3fe0 to 0x40000000, burst size: 32 bytes
rw throughput: 18917.42 KB/s
compare data between source address 0x10c3fe0 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3ff0 to 0x40000000, burst size: 16 bytes
rw throughput: 15469.52 KB/s
compare data between source address 0x10c3ff0 and destination address 0x40000000:  all data matches
dma transferring data from 0x10c3ff8 to 0x40000000, burst size: 8 bytes
rw throughput: 11923.98 KB/s
compare data between source address 0x10c3ff8 and destination address 0x40000000:  all data matches
read testing
dma transferring data from 0x40002000 to 0x10c0000, burst size: 8192 bytes
rw throughput: 177142.20 KB/s
compare data between source address 0x40002000 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003000 to 0x10c0000, burst size: 4096 bytes
rw throughput: 88550.67 KB/s
compare data between source address 0x40003000 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003800 to 0x10c0000, burst size: 2048 bytes
rw throughput: 59023.57 KB/s
compare data between source address 0x40003800 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003c00 to 0x10c0000, burst size: 1024 bytes
rw throughput: 44260.66 KB/s
compare data between source address 0x40003c00 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003e00 to 0x10c0000, burst size: 512 bytes
rw throughput: 35408.02 KB/s
compare data between source address 0x40003e00 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003f00 to 0x10c0000, burst size: 256 bytes
rw throughput: 29484.60 KB/s
compare data between source address 0x40003f00 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003f80 to 0x10c0000, burst size: 128 bytes
rw throughput: 25221.26 KB/s
compare data between source address 0x40003f80 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003fc0 to 0x10c0000, burst size: 64 bytes
rw throughput: 21125.60 KB/s
compare data between source address 0x40003fc0 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003fe0 to 0x10c0000, burst size: 32 bytes
rw throughput: 17510.96 KB/s
compare data between source address 0x40003fe0 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003ff0 to 0x10c0000, burst size: 16 bytes
rw throughput: 13945.32 KB/s
compare data between source address 0x40003ff0 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40003ff8 to 0x10c0000, burst size: 8 bytes
rw throughput: 10268.72 KB/s
compare data between source address 0x40003ff8 and destination address 0x10c0000:  all data matches
chained transfer
 dma setup channel done
compare data between source address 0x10c0000 and destination address 0x40000000:  all data matches
 [0]: data match
compare data between source address 0x10c1000 and destination address 0x40020000:  all data matches
 [1]: data match
compare data between source address 0x10c2000 and destination address 0x40040000:  all data matches
 [2]: data match
compare data between source address 0x10c3000 and destination address 0x40060000:  all data matches
 [3]: data match
dma example end

```