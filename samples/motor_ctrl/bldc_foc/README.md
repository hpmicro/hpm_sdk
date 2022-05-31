# BLDC FOC Control

## Overview

The **bldc_foc** project contains the speed control, position control, and sensorless speed control of DC brushless motors.
- **freemaster** for speed control, mode switching, position control
- Use the **FOC** control algorithm
- Sensorless control algorithm for **SMO**

## Configurations

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", please refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.

- Board settings refer to the development board documentation [ PWM_PIN ](lab_board_motor_ctrl_pin) related content

- Click [DRV-LV50A-MP1907 Motor Driver Board](lab_drv_lv50a_mp1907) section and configure

- **freemaster** settings refer to [freemaster configuration](lab_samples_freemaster_configure) section
  - **freemaster** project file address `./bldc_foc_segger.pmpx`

## Code Options

- Add `sdk_compile_options("-mabi=ilp32f")` to the project's CMakeLists.txt to get a higher performance
- Set `#define MOTOR0_SMC_EN (0) /*enable sliding mode control*/` in bldc_foc.c to 1 to enable SMO.

```{note}

This code does not support any compilation optimization, please turn off compilation optimization

```


## Running the demo
### With sensor

The motor runs at a speed of 20r/s.
The following data can be configured by clicking on the GO icon of **freemaster**：

`fre_setspeed` float, range +40~-40, unit r/s

`fre_setpos` int, (-4000, +4000) corresponds to (-360, +360) degrees

`fre_user_mode` bool, 1- Speed mode 0- Position mode

Check the following parameters

`fre_dispspeed` float, unit r/s

- Speed mode

	Enter the correct speed parameters and then observe the speed change with **freemaster**.

- Position mode

	Set the position of the motor's shaft, then the motor's shaft will be locked at the specified position


### Sensorless

The motor runs at a speed of 20r/s.
The following data can be configured by clicking on the GO icon of **freemaster**：

`fre_setspeed` float, range (-40, -5) and (5, +40), unit r/s

Check the following parameters

`fre_dispspeed` float, unit r/s

```{warning}

- After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter.

- Pay attention to the current size, when abnormalities occur, please cut off the power at any time

- Use the **gnu-gcc** toolchain and load the `. /bldc_foc_segger.pmpx` file for more features

```
