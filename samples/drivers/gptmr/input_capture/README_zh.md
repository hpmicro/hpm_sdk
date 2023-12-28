# GPTMR输入捕获例子

## 概述

input_captrue示例工程演示了GPTMR输入捕获的功能. 

## 硬件设置

请参考具体开发板{ref}`引脚描述 <lab_board_resource>`部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

## 运行现象

运行工程，示例执行GPTMR的输入捕获的功能，串口终端输出如下信息，打印出每次捕获到的边沿间隔时间,也就是一个周期的时间。
```console
gptmr input capture test
capture the pulse width : 100001 tick
capture the pulse width : 100000 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100000 tick
capture the pulse width : 100000 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick
capture the pulse width : 100000 tick
capture the pulse width : 100001 tick
capture the pulse width : 100001 tick

```