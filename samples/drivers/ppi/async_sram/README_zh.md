# PPI ASYNC SRAM example
## 概述

本示例工程展示了使用PPI外设读写外部异步SRAM存储器。

PPI外设的地址空间为：0xF8000000~0xFFFFFFFF，基地址需1MB对齐。

## 硬件设置

需外接异步SRAM模块，本sample测试的异步SRAM型号为：IS62WV51216EBLL45TLI。AD复用模式时，本sample使用的地址锁存器型号为：SN74HC573APWR。

PPI组件支持AD复用模式和AD非复用模式。

[1] 使用地址和数据非复用模式时，`DQ0-DQ15`作为数据线，`DQ16-DQ31`作为地址线。IS62WV51216EBLL45TLI的高位地址A16以上的须接地。
[2] 使用地址和数据复用模式时，`DQ0-DQ15`作为数据线和地址线。IS62WV51216EBLL45TLI的高位地址A16以上的须接地。

本示例中，使用宏`BOARD_PPI_ASYNC_SRAM_AD_MUX_MODE`选择AD的工作模式，使用宏`BOARD_PPI_ASYNC_SRAM_CS_INDEX`选择CS信号，使用宏`BOARD_PPI_ASYNC_SRAM_ADV_CTRL_PIN`选择ADV信号，使用宏`BOARD_PPI_ASYNC_SRAM_OE_CTRL_PIN`选择REL信号，使用宏`BOARD_PPI_ASYNC_SRAM_WE_CTRL_PIN`选择WEL信号。用户可根据自己实际原理图进行修改。

PPI组件的更多信息，请参考：[PPI组件Readme.md](../../../../components/ppi/README_zh.md)

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
sram example
comparison test: from 0xf8000000 to 0xf8020000
**  u32 comparison succeeded
**  u16 comparison succeeded
**  u8 comparison succeeded
```
