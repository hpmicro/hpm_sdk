# BLDC FOC控制

## 概述

**bldc_foc**工程展示了直流无刷电机的有感转速控制、位置控制.
- 电机控制算法为**FOC**

## 配置

- 本例程电机使用的是雷赛智能的 **BLM57050-1000** 无刷电机，电机具体参数请参考[雷赛智能官网](https://leisai.com/)。

- 板子设置参考开发板文档[Motor Pin](lab_board_motor_ctrl_pin)相关内容

- 完成上述过程后，给驱动板上电观察电流无异常后，给核心板上电，再次确认电流无异常后，就可以烧录程序，观察运行现象。

## 电流环时间

硬件电流环会显示时间零，软件电流环计算时间在1us左右，这个时间会根据角度不同而波动，波动范围在25%，通过进行如下操作复现：

- 关闭`mcl_app_config.h`除`MCL_EN_LOOP_TIME_COUNT`的宏
- 编译选项`flash_xip_release`

## 运行现象

当工程正确运行后，电机以20r/s的速度运行。
通过串口控制台可配置如下参数：

`speed` float类型， 输入范围+40~-40，单位r/s

`pos` int类型，范围不限，单位(-4000,+4000)对应(-360,+360)度

`mode` bool类型， 1- 速度模式   0-位置模式

- 速度模式：

	可以在规定的速度范围内配置速度

- 位置模式：

	可以设置电机轴的位置，此时电机轴会锁定在指定的位置


```console

loop current tick: 399
Mode selection:
0. Location mode.
1. Speed mode.
Enter mode code:
1
Speed mode, motor run, speed is: 20.000000.
Input speed:
10.5

loop current tick: 535
Speed mode, motor run, speed is: 10.500000.
Input speed:

```

```{warning}

- 电机上电后首先要完成对中动作，这时候请不要干预电机运行，否则会产生抖动

- 电机运行时，请时刻注意电流大小，如果发生异常，请随时准备切断电源。

- 输入速度和位置的数值时，需要输入换行符作为输入结束标志

```
