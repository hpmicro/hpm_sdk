# GPIO

## 概述

GPIO示例工程演示了控制GPIO引脚输出电平变化和输入引脚触发中断的功能。

## 硬件设置

无特殊设置

## 已知问题

当GPIO用作按键输入引脚检测时，受按键机械抖动影响，输入电平抖动，会造成GPIO多次检测到输入电平变化的情况。

## 运行现象
- 支持双沿触发模式的芯片，工程配置的是双沿触发模式，每次按键按下和释放都会翻转一次LED状态，并打印两次LED切换输出。
- 当工程正确运行后，可以观察到LED闪烁5次，之后按下GPIO按键(请确认具体开发板 {ref}`按键 <lab_board_resource>` 部分描述)可以翻转LED的状态，串口终端会输出如下信息：
```console
toggling led 5 times in total
toggling led 1/5 times
toggling led 2/5 times
toggling led 3/5 times
toggling led 4/5 times
toggling led 5/5 times
input interrupt
user led will be switched on off based on user switch
toggle led pin output
toggle led pin output
```
