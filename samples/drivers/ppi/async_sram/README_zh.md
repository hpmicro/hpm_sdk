# PPI ASYNC SRAM example
## 概述

本示例工程展示了使用PPI外设读写外部异步SRAM存储器。

PPI外设的地址空间为：0xF8000000~0xFFFFFFFF，基地址需1MB对齐。

## 硬件设置

需外接异步SRAM模块，本sample测试的异步SRAM型号为：IS62WV51216EBLL45TLI。

使用地址和数据非复用模式，DQ0~DQ15作为数据线，DQ16~DQ31作为地址线。IS62WV51216EBLL45TLI的高位地址A16以上的须接地。

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
sram example
comparison test: from 0xf8000000 to 0xf8020000
**  u32 comparison succeeded
**  u16 comparison succeeded
**  u8 comparison succeeded
```
