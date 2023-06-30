# BLDC FOC Control

## Overview

The **bldc_foc** project contains the speed control and position control of DC brushless motors.
- Use the **FOC** control algorithm

## Configurations

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", please refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.

- Board settings refer to the development board documentation [Motor Pin](lab_board_motor_ctrl_pin) related content

## Code Options

- Add `sdk_compile_options("-mabi=ilp32f")` to the project's CMakeLists.txt to get a higher performance

```{note}

This code does not support any compilation optimization, please turn off compilation optimization

```


## Running the demo
### With sensor

The motor runs at a speed of 20r/s.
The following data can be configured by serial console ：

`speed` float, range +40~-40, unit r/s

`pos` int, (-4000, +4000) corresponds to (-360, +360) degrees

`mode` bool, 1- Speed mode 0- Position mode

- Speed mode

	Enter the correct speed parameters with serial console.

- Position mode

	Set the position of the motor's shaft, then the motor's shaft will be locked at the specified position

```console
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

- After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter.

- Pay attention to the current size, when abnormalities occur, please cut off the power at any time

- When entering values for speed and position, a line break is required as an end-of-entry flag

```
