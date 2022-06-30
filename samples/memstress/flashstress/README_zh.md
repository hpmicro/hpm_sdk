# FLASHSTERSS example

## 概述

FLASHSTERSS 示例工程对外部的NOR Flash进行压力测试. 主要包含如下测试内容:

**使能 dcache 并且使用 API 读**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

**使能 dcache 并且使用内存总线读**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

**禁止 dcache 并且使用 API 读**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

**禁止 dcache 并且使用内存总线读**

- erase chip (earse whole chip in once)
- write 0x55
- read verify
- erase all block (earse one block until all block be erased)
- write 0xAA
- read verify

## Board Setting

无需特殊设置

## Running the example

当工程正确运行后，串口终端会输出如下信息：

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

> *注意: 这个工程大约需要花费 10 分钟.请耐心等待!
