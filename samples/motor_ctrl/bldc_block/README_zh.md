# BLDC梯形波控制
## 概述

**bldc_block**工程展示了直流无刷电机的转速控制。
- 通过梯形波算法进行控制

## 配置

- 本例程电机使用的是雷赛智能的 **BLM57050-1000** 无刷电机，电机具体参数请参考[雷赛智能官网](https://leisai.com/)。

- 板子设置参考开发板文档[ PWM_PIN ](lab_board_motor_ctrl_pin)相关内容

- 支持[HPM6750EVKMINI-TO_MOTOR电机扩展板](lab_board_hpm6750evkmini-to-motor-220530RevA)的开发板参考相关章节进行配置，支持列表如下：
  - hpm6750evkmini

- 支持[DRV-LV50A-MP1907电机驱动板](lab_drv_lv50a_mp1907)的开发板参考相关章节进行配置，支持列表如下:
  - hpm6750evk
  - hpm6360evk

- 完成上述过程后，给驱动板上电观察电流无异常后，给核心板上电，再次确认电流无异常后，就可以烧录程序，观察运行现象。

```{note}

本代码不支持任何编译优化，需要手动关闭编译优化

```

## 运行现象

当工程正确运行后，电机以20r/s的速度运行。
通过串口控制台可配置如下参数：

``speed`` float类型， 输入范围+40~-40，单位r/s

串口输出如下：

```
Mode selection:
0. Location mode.
1. Speed mode.
Enter mode code:
1
Speed mode, motor run, speed is: 20.000000.
Input speed:
10.5

Speed mode, motor run, speed is: 10.500000.
Input speed:

```

```{warning}
电机运行时，请时刻注意电流大小，如果发生异常，请随时准备切断电源
```
