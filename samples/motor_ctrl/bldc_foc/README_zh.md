# BLDC FOC控制
## 概述

**bldc_foc**工程展示了直流无刷电机的有感转速控制、位置控制，
以及无感转速控制。
- 通过**freemaster**进行转速控制，模式切换，位置控制
- 电机控制算法为**FOC**
- 无感控制算法为**SMO**

## 配置

- 本例程电机使用的是雷赛智能的 **BLM57050-1000** 无刷电机，电机具体参数请参考[雷赛智能官网](https://leisai.com/)。

- 板子设置参考开发板文档[ PWM_PIN ](lab_board_motor_ctrl_pin)相关内容

- 点击查看[DRV-LV50A-MP1907电机驱动板](lab_drv_lv50a_mp1907)章节并进行配置

- 设置**freemaster**请参考[freemaster 配置](lab_samples_freemaster_configure)章节
	- **freemaster**工程文件地址`./bldc_foc_segger.pmpx`

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
点击**freemaster**的GO图标，启动freemaster即可配置如下数据：

`fre_setspeed` float类型， 输入范围+40~-40，单位r/s

`fre_setpos` int类型，范围不限，单位(-4000,+4000)对应(-360,+360)度

`fre_user_mode` bool类型， 1- 速度模式   0-位置模式

- 速度模式：

	可以在规定的速度范围内输入速度参数，并通过**freemaster**观察速度变化

- 位置模式：

	可以设置电机轴的位置，此时电机轴会锁定在指定的位置

也可以查看如下参数：

	`fre_dispspeed` float类型，单位r/s

### 无感

当工程正确运行后，电机以20r/s的速度运行。
点击**freemaster**的GO图标，启动freemaster即可配置如下数据：

`fre_setspeed` float类型， 输入范围+40~+5，-5~-40单位r/s

也可以查看如下参数：

`fre_dispspeed` float类型，单位r/s

```{warning}

- 电机上电后首先要完成对中动作，这时候请不要干预电机运行，否则会产生抖动

- 电机运行时，请时刻注意电流大小，如果发生异常，请随时准备切断电源。

- 如果需要功能更多的可配置项，请使用我们提供的**gnu-gcc**工具链，并通过**freemaster**
载入地址`./bldc_foc.pmpx`的文件

```
