# UART软件接收空闲检测

## 概述

该示例工程，UART使用软件接受空闲检测通过DMA接收不定长的数据（数据量小于程序定义的BUFF_SIZE）。
该方法用于UART硬件不支持接收空闲检测的情况，通过TRGMUX和GPTMR软件检测RX信号的方式实现接收空闲检测。
如果硬件支持接收空闲检测，则可使用uart_hardware_rx_idle的例程。

## 端口设置

-  串口波特率设置为``115200bps``，``1个停止位``，``无奇偶校验位``

## 硬件设置
- 连接UART发送器的TX与开发板上的RX， 连接UART发送器的GND与开发板的GND
- 连接开发板的RX与TRGMUX的输入IO

## 运行现象

当工程正确运行后，通过UART发送器终端发送数据，例如"1234567890123", 调试串口终端会以如下形式输出信息：
```console
uart software rx idle detection.
uart receive 13 bytes, the received data are:
1 2 3 4 5 6 7 8 9 0
1 2 3
```

