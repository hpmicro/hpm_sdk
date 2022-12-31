# SPI_POLLING_MASTER
## 概述

spi_polling_master示例工程展示了SPI作为master使用polling方法进行板与板之间通信。
在这个示例工程中， 一个板子作为SPI master， 另一个板子作为SPI slave。 Master发送一定量数据到Slave并从Slave接收一定量的数据。

## 硬件设置

将两个板子的[SPI引脚](lab_board_app_spi_pin)相连。


## 运行现象
- 先运行slave，再运行master

- 当工程正确运行后，串口终端会输出如下信息：
```
SPI-Master Polling Transfer Example
SPI-Master transfer timing is configured.
SPI-Master transfer source clock frequency: 24000000Hz
SPI-Master tannsfer sclk frequecny: 1562500Hz
SPI-Master transfer format is configured.
SPI-Master transfer mode:write-read-together
SPI-Master transfer starts.
SPI-Master write data: 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9
SPI-Master read  data: 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9
SPI-Master transfer ends.
```

