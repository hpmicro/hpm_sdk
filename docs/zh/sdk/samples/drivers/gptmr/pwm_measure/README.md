# GPTMR测量PWM

## 概述

pwm_measure示例工程演示了GPTMR测量PWM波的功能。

## 硬件设置

请参考具体开发板{ref}`引脚描述 <lab_board_resource>`部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

## 运行现象

运行工程，示例执行GPTMR的测量PWM波的功能，若有PWM波输入，串口终端输出如下信息，分别是PWM波的频率和占空比。
```console
gptmr pwm measure test
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
```


