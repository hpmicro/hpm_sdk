# SPI_INTERRUPT_MASTER
## 概述

spi_interrupt_master示例工程展示了SPI作为master使用interrupt方法进行板与板之间通信。
在这个示例工程中， 一个板子作为SPI master， 另一个板子作为SPI slave。 Master发送一定量数据到Slave并从Slave接收一定量的数据。

## 硬件设置

将两个板子的[SPI引脚](lab_board_app_spi_pin)相连。


## 运行现象
- 先运行slave，再运行master

- 当工程正确运行后，串口终端会输出如下信息：
```
SPI-Master Interrupt Transfer Example
SPI-Master transfer timing is configured.
SPI-Master transfer source clock frequency: 24000000Hz
SPI-Master tannsfer sclk frequecny: 1562500Hz
SPI-Master transfer format is configured.
SPI master sent data:
0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 0xA6 0xA7 0xA8 0xA9
SPI master receive data:
0xB0 0xB1 0xB2 0xB3 0xB4 0xB5 0xB6 0xB7 0xB8 0xB9
SPI master transfer done.

```

