# GPTMR
## 概述

GPTMR示例工程演示了GPTMR输出比较、输入捕获、PWM测量和生成PWM的功能。

## 硬件设置

请参考具体开发板{ref}`引脚描述 <lab_board_resource>`部分的描述。
连接外部PWM至GPTMR输入捕获引脚，连接GPTMR比较输出引脚至示波器。

## 运行现象

运行工程，示例依次执行GPTMR的输出比较， 输入捕获， PWM测量和生成PWM的功能，串口终端输出如下信息，使用示波器观测输出的占空比不断变化的PWM：
```
> timer testing
> time is up: 0x1059
> captured rising edge: 0x36f3f
> measured period: 0x10001
> measured duty cycle: 0x8000
> test generate waveform edge aligned pwm
```
