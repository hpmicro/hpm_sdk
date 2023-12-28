# GPTMR采集解析sent

## 概述

sent_signal_decode示例工程演示了GPTMR的测量PWM功能并且使用DMA来采集sent信号

## 硬件设置

请参考具体开发板{ref}`引脚描述 <lab_board_resource>`部分的描述。
连接sent信号至GPTMR输入捕获引脚。

## 运行现象

运行工程，示例执行采集sent信号，若有sent信号输入，串口终端输出如下信息，分别对应的一帧完整的sent协议帧。
```console
sent signal decode demo
stat:00 data:01 02 03 04 05 06 crc:02  pause:false

stat:00 data:02 03 06 04 08 crc:04  pause:false

stat:00 data:02 03 06 08 09 0a crc:09  pause:false

stat:00 data:01 02 03 04 05 06 crc:02  pause:false

stat:00 data:02 03 06 04 08 crc:04  pause:false

stat:00 data:02 03 06 08 09 0a crc:09  pause:false

stat:00 data:01 01 02 crc:0b  pause:false

stat:00 data:02 03 05 06 crc:0e  pause:false


```


