# QEOV2_SEI2ABZ

## 概述

QEOV2_SEI2ABZ示例工程展示通过SEI接口获取传感器位置信息(本示例使用多摩川传感器TS5700N8401)，根据SEI获取的位置信息，动态的调整QEO输出ABZ信号的宽度，实现均匀输出ABZ信号。

## 硬件设置

- 连接多摩川传感器TS5700N8401至开发板对应引脚DATA_P/DATA_N，并5V供电。
- 连接开发板ABZ输出引脚QEO_A/QEO_B/_QEO_Z至逻辑分析仪抓取输出波形(请参考{ref}`引脚描述 <lab_board_resource>`部分)。


## 运行现象

- 当工程正确运行后，转动传感器，逻辑分析仪会捕捉到与转动状态相一致的均匀的ABZ信号，串口终端会输出如下信息：
```console
QEO SEI2ABZ example
QEO generate ABZ signal with hardware(SEI) provide postion
device init finish
```

