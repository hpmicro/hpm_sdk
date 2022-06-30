# FLASHSTERSS example

## Overview

FLASHSTERSS example project stress test for external NOR Flash. It's contain the follow:

**dcache enable and read by api**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

**dcache enable and read by mem bus**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

**dcache disable  and read by api**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

**dcache disable  and read by mem bus**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:

```bash
=============================================
[read api(dcache enable)]: chip size: 16777216
[read api(dcache enable)]: block size: 65536
=============================================
[read api(dcache enable)]: [erase_chip] PASSED!!!
[read api(dcache enable)]: [erase_chip] take 22.439308 seconds!!!
[read api(dcache enable)]: [write] PASSED!!!
[read api(dcache enable)]: [write] take time: 30.603752, speed: 0.522812 MiB/s
[read api(dcache enable)]: [read] PASSED!!!
[read api(dcache enable)]: [read] take time: 0.599961, speed: 26.668401 MiB/s
[read api(dcache enable)]: [erase all blocks] PASSED
[read api(dcache enable)]: [erase all blocks] take time: 31.895067 seconds
[read api(dcache enable)]: [write] PASSED!!!
[read api(dcache enable)]: [write] take time: 30.333946, speed: 0.527462 MiB/s
[read api(dcache enable)]: [read] PASSED!!!
[read api(dcache enable)]: [read] take time: 0.599961, speed: 26.668401 MiB/s

=============================================
[read mem(dcache enable)]: chip size: 16777216
[read mem(dcache enable)]: block size: 65536
=============================================
[read mem(dcache enable)]: [erase_chip] PASSED!!!
[read mem(dcache enable)]: [erase_chip] take 22.510139 seconds!!!
[read mem(dcache enable)]: [write] PASSED!!!
[read mem(dcache enable)]: [write] take time: 30.774879, speed: 0.519905 MiB/s
[read mem(dcache enable)]: [read] PASSED!!!
[read mem(dcache enable)]: [read] take time: 0.445520, speed: 35.913089 MiB/s
[read mem(dcache enable)]: [erase all blocks] PASSED
[read mem(dcache enable)]: [erase all blocks] take time: 31.973159 seconds
[read mem(dcache enable)]: [write] PASSED!!!
[read mem(dcache enable)]: [write] take time: 30.507415, speed: 0.524463 MiB/s
[read mem(dcache enable)]: [read] PASSED!!!
[read mem(dcache enable)]: [read] take time: 0.445585, speed: 35.907852 MiB/s

=============================================
[read api(dcache disable)]: chip size: 16777216
[read api(dcache disable)]: block size: 65536
=============================================
[read api(dcache disable)]: [erase_chip] PASSED!!!
[read api(dcache disable)]: [erase_chip] take 22.774911 seconds!!!
[read api(dcache disable)]: [write] PASSED!!!
[read api(dcache disable)]: [write] take time: 30.924932, speed: 0.517382 MiB/s
[read api(dcache disable)]: [read] PASSED!!!
[read api(dcache disable)]: [read] take time: 0.600448, speed: 26.646770 MiB/s
[read api(dcache disable)]: [erase all blocks] PASSED
[read api(dcache disable)]: [erase all blocks] take time: 32.059715 seconds
[read api(dcache disable)]: [write] PASSED!!!
[read api(dcache disable)]: [write] take time: 30.684532, speed: 0.521435 MiB/s
[read api(dcache disable)]: [read] PASSED!!!
[read api(dcache disable)]: [read] take time: 0.600447, speed: 26.646814 MiB/s

=============================================
[read mem(dcache disable)]: chip size: 16777216
[read mem(dcache disable)]: block size: 65536
=============================================
[read mem(dcache disable)]: [erase_chip] PASSED!!!
[read mem(dcache disable)]: [erase_chip] take 22.583667 seconds!!!
[read mem(dcache disable)]: [write] PASSED!!!
[read mem(dcache disable)]: [write] take time: 31.061794, speed: 0.515102 MiB/s
[read mem(dcache disable)]: [read] PASSED!!!
[read mem(dcache disable)]: [read] take time: 0.439910, speed: 36.371074 MiB/s
[read mem(dcache disable)]: [erase all blocks] PASSED
[read mem(dcache disable)]: [erase all blocks] take time: 32.009662 seconds
[read mem(dcache disable)]: [write] PASSED!!!
[read mem(dcache disable)]: [write] take time: 30.804359, speed: 0.519407 MiB/s
[read mem(dcache disable)]: [read] PASSED!!!
[read mem(dcache disable)]: [read] take time: 0.439910, speed: 36.371074 MiB/s

=============================================
All cases are PASSED
=============================================
```

> *notes: The project takes about 10 mintues. wait patiently, please!*

