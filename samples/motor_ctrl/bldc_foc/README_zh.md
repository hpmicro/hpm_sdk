# BLDC FOC控制
## 概述

**bldc_foc**工程展示了直流无刷电机的有感转速控制、位置控制，
以及无感转速控制。
- 电机控制算法为**FOC**
- 无感控制算法为**SMO**

## 配置

- 本例程电机使用的是雷赛智能的 **BLM57050-1000** 无刷电机，电机具体参数请参考[雷赛智能官网](https://leisai.com/)。

- 板子设置参考开发板文档[ PWM_PIN ](lab_board_motor_ctrl_pin)相关内容

- 支持[HPM6750EVKMINI-TO_MOTOR电机扩展板](lab_board_hpm6750evkmini-to-motor-220530RevA)的开发板参考相关章节进行配置，支持列表如下：
  - hpm6750evkmini

- 支持[DRV-LV50A-MP1907电机驱动板](lab_drv_lv50a_mp1907)的开发板参考相关章节进行配置，支持列表如下:
  - hpm6750evk
  - hpm6360evk

- 完成上述过程后，给驱动板上电观察电流无异常后，给核心板上电，再次确认电流无异常后，就可以烧录程序，观察运行现象。

## 代码选项

- 如果需要更快的代码运行速度请在该工程的CMakeLists.txt中添加`sdk_compile_options("-mabi=ilp32f")`语句，获得较高的性能提升
- 本代码包含无传感器的滑模控制算法，请将 bldc_foc.c 中的 `#define MOTOR0_SMC_EN               (0) /*使能滑模控制*/`置1即可。

```{note}

本代码不支持任何编译优化，需要手动关闭编译优化

```

## 运行现象

### 有感

当工程正确运行后，电机以20r/s的速度运行。
通过串口控制台可配置如下参数：

`speed` float类型， 输入范围+40~-40，单位r/s

`pos` int类型，范围不限，单位(-4000,+4000)对应(-360,+360)度

`mode` bool类型， 1- 速度模式   0-位置模式

- 速度模式：

	可以在规定的速度范围内配置速度

- 位置模式：

	可以设置电机轴的位置，此时电机轴会锁定在指定的位置



### 无感

当工程正确运行后，电机以20r/s的速度运行。
通过串口控制台可配置如下数据：

`speed` float类型， 输入范围+40~+5，-5~-40单位r/s

串口输出如下：

```
Mode selection:
1. Location mode.
2. Speed mode.
Enter mode code:

Location mode, motor run, The location is: 0.
Input Location:
 10000

Location mode, motor run, The location is: 10000.
Input Location:

```

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

- 电机上电后首先要完成对中动作，这时候请不要干预电机运行，否则会产生抖动

- 电机运行时，请时刻注意电流大小，如果发生异常，请随时准备切断电源。

- 输入速度和位置的数值时，需要输入换行符作为输入结束标志

```
