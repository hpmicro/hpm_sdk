# FreeRTOS Hello Gptmr

## 概述

该工程演示了freertos的任务调度功能,使用gptmr代替了mtmr产生系统中断。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，LED会开始闪烁，串口终端会输出如下信息：
```console
Toggle the led pin level.
task2 is running.
LED is off.
LED is off.
LED is off.
LED is off.
task2 is running.
Toggle the led pin level.
LED is on.
LED is on.
LED is on.
task2 is running.
Toggle the led pin level.
LED is off.
LED is off.
LED is off.
LED is off.
```
