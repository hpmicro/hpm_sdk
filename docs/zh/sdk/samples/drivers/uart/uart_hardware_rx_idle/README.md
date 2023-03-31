# UART硬件接收空闲检测

## 概述

该示例工程，UART使用硬件接受空闲检测功能通过DMA接收不定长的数据（数据量小于程序定义的BUFF_SIZE）。

## 端口设置

-  串口波特率设置为``115200bps``，``1个停止位``，``无奇偶校验位``

## 硬件设置
- 无特殊配置

## 运行现象

当工程正确运行后，通过串口中断输入一些数据，串口终端会以如下形式输出信息：
```console
uart hardware rx idle detection.
uart receive 13 bytes, the received data are:
1 2 3 4 5 6 7 8 9 0
1 2 3
```

