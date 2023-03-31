# DMA example

## Overview

DMA example project shows transfer data with unchined and chained way. This example checks if the data transferred by DMA is correct.

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```console
dma example start
unchained transfer
write testing
dma transferring data from 0x82000 to 0x1100000, burst size: 8192 bytes
rw throughput: 534818.93 KB/s
compare data between source address 0x82000 and destination address 0x1100000:  all data matches
dma transferring data from 0x83000 to 0x1100000, burst size: 4096 bytes
rw throughput: 535191.62 KB/s
compare data between source address 0x83000 and destination address 0x1100000:  all data matches
dma transferring data from 0x83800 to 0x1100000, burst size: 2048 bytes
rw throughput: 534446.75 KB/s
compare data between source address 0x83800 and destination address 0x1100000:  all data matches
dma transferring data from 0x83c00 to 0x1100000, burst size: 1024 bytes
rw throughput: 532593.62 KB/s
compare data between source address 0x83c00 and destination address 0x1100000:  all data matches
dma transferring data from 0x83e00 to 0x1100000, burst size: 512 bytes
rw throughput: 528561.62 KB/s
compare data between source address 0x83e00 and destination address 0x1100000:  all data matches
dma transferring data from 0x83f00 to 0x1100000, burst size: 256 bytes
rw throughput: 520677.96 KB/s
compare data between source address 0x83f00 and destination address 0x1100000:  all data matches
dma transferring data from 0x83f80 to 0x1100000, burst size: 128 bytes
rw throughput: 504930.96 KB/s
compare data between source address 0x83f80 and destination address 0x1100000:  all data matches
dma transferring data from 0x83fc0 to 0x1100000, burst size: 64 bytes
rw throughput: 381520.12 KB/s
compare data between source address 0x83fc0 and destination address 0x1100000:  all data matches
dma transferring data from 0x83fe0 to 0x1100000, burst size: 32 bytes
rw throughput: 256341.78 KB/s
compare data between source address 0x83fe0 and destination address 0x1100000:  all data matches
dma transferring data from 0x83ff0 to 0x1100000, burst size: 16 bytes
rw throughput: 154776.29 KB/s
compare data between source address 0x83ff0 and destination address 0x1100000:  all data matches
dma transferring data from 0x83ff8 to 0x1100000, burst size: 8 bytes
rw throughput: 86360.06 KB/s
compare data between source address 0x83ff8 and destination address 0x1100000:  all data matches
read testing
dma transferring data from 0x1102000 to 0x80000, burst size: 8192 bytes
rw throughput: 523160.75 KB/s
compare data between source address 0x1102000 and destination address 0x80000:  all data matches
dma transferring data from 0x1103000 to 0x80000, burst size: 4096 bytes
rw throughput: 523517.37 KB/s
compare data between source address 0x1103000 and destination address 0x80000:  all data matches
dma transferring data from 0x1103800 to 0x80000, burst size: 2048 bytes
rw throughput: 522804.62 KB/s
compare data between source address 0x1103800 and destination address 0x80000:  all data matches
dma transferring data from 0x1103c00 to 0x80000, burst size: 1024 bytes
rw throughput: 521031.21 KB/s
compare data between source address 0x1103c00 and destination address 0x80000:  all data matches
dma transferring data from 0x1103e00 to 0x80000, burst size: 512 bytes
rw throughput: 516129.03 KB/s
compare data between source address 0x1103e00 and destination address 0x80000:  all data matches
dma transferring data from 0x1103f00 to 0x80000, burst size: 256 bytes
rw throughput: 507600.78 KB/s
compare data between source address 0x1103f00 and destination address 0x80000:  all data matches
dma transferring data from 0x1103f80 to 0x80000, burst size: 128 bytes
rw throughput: 486383.78 KB/s
compare data between source address 0x1103f80 and destination address 0x80000:  all data matches
dma transferring data from 0x1103fc0 to 0x80000, burst size: 64 bytes
rw throughput: 366412.21 KB/s
compare data between source address 0x1103fc0 and destination address 0x80000:  all data matches
dma transferring data from 0x1103fe0 to 0x80000, burst size: 32 bytes
rw throughput: 246232.76 KB/s
compare data between source address 0x1103fe0 and destination address 0x80000:  all data matches
dma transferring data from 0x1103ff0 to 0x80000, burst size: 16 bytes
rw throughput: 147041.93 KB/s
compare data between source address 0x1103ff0 and destination address 0x80000:  all data matches
dma transferring data from 0x1103ff8 to 0x80000, burst size: 8 bytes
rw throughput: 81606.64 KB/s
compare data between source address 0x1103ff8 and destination address 0x80000:  all data matches
chained transfer
 dma setup channel done
compare data between source address 0x1040000 and destination address 0x1100000:  all data matches
 [0]: data match
compare data between source address 0x1041000 and destination address 0x1108000:  all data matches
 [1]: data match
compare data between source address 0x1042000 and destination address 0x1110000:  all data matches
 [2]: data match
dma example end


```
