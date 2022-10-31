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
dma transferring data from 0x10c2000 to 0x40100000, burst size: 8192 bytes
rw throughput: 191056.66 KB/s
compare data between source address 0x10c2000 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3000 to 0x40100000, burst size: 4096 bytes
rw throughput: 191401.87 KB/s
compare data between source address 0x10c3000 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3800 to 0x40100000, burst size: 2048 bytes
rw throughput: 191068.54 KB/s
compare data between source address 0x10c3800 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3c00 to 0x40100000, burst size: 1024 bytes
rw throughput: 190629.85 KB/s
compare data between source address 0x10c3c00 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3e00 to 0x40100000, burst size: 512 bytes
rw throughput: 189993.20 KB/s
compare data between source address 0x10c3e00 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3f00 to 0x40100000, burst size: 256 bytes
rw throughput: 188316.07 KB/s
compare data between source address 0x10c3f00 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3f80 to 0x40100000, burst size: 128 bytes
rw throughput: 185283.47 KB/s
compare data between source address 0x10c3f80 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3fc0 to 0x40100000, burst size: 64 bytes
rw throughput: 142678.00 KB/s
compare data between source address 0x10c3fc0 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3fe0 to 0x40100000, burst size: 32 bytes
rw throughput: 115077.73 KB/s
compare data between source address 0x10c3fe0 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3ff0 to 0x40100000, burst size: 16 bytes
rw throughput: 84911.14 KB/s
compare data between source address 0x10c3ff0 and destination address 0x40100000:  all data matches
dma transferring data from 0x10c3ff8 to 0x40100000, burst size: 8 bytes
rw throughput: 53096.43 KB/s
compare data between source address 0x10c3ff8 and destination address 0x40100000:  all data matches
read testing
dma transferring data from 0x40102000 to 0x10c0000, burst size: 8192 bytes
rw throughput: 178014.72 KB/s
compare data between source address 0x40102000 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103000 to 0x10c0000, burst size: 4096 bytes
rw throughput: 177757.20 KB/s
compare data between source address 0x40103000 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103800 to 0x10c0000, burst size: 2048 bytes
rw throughput: 177603.05 KB/s
compare data between source address 0x40103800 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103c00 to 0x10c0000, burst size: 1024 bytes
rw throughput: 177295.55 KB/s
compare data between source address 0x40103c00 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103e00 to 0x10c0000, burst size: 512 bytes
rw throughput: 177275.09 KB/s
compare data between source address 0x40103e00 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103f00 to 0x10c0000, burst size: 256 bytes
rw throughput: 176714.22 KB/s
compare data between source address 0x40103f00 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103f80 to 0x10c0000, burst size: 128 bytes
rw throughput: 174674.48 KB/s
compare data between source address 0x40103f80 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103fc0 to 0x10c0000, burst size: 64 bytes
rw throughput: 131035.66 KB/s
compare data between source address 0x40103fc0 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103fe0 to 0x10c0000, burst size: 32 bytes
rw throughput: 102444.39 KB/s
compare data between source address 0x40103fe0 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103ff0 to 0x10c0000, burst size: 16 bytes
rw throughput: 68524.01 KB/s
compare data between source address 0x40103ff0 and destination address 0x10c0000:  all data matches
dma transferring data from 0x40103ff8 to 0x10c0000, burst size: 8 bytes
rw throughput: 38964.99 KB/s
compare data between source address 0x40103ff8 and destination address 0x10c0000:  all data matches
chained transfer
 dma setup channel done
compare data between source address 0x10c0000 and destination address 0x40100000:  all data matches
 [0]: data match
compare data between source address 0x10c1000 and destination address 0x40120000:  all data matches
 [1]: data match
compare data between source address 0x10c2000 and destination address 0x40140000:  all data matches
 [2]: data match
compare data between source address 0x10c3000 and destination address 0x40160000:  all data matches
 [3]: data match
dma example end

```