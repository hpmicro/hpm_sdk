# BLDC Trapezoidal Wave Control
## Overview

**Bldc_block** project demonstrates speed control of the BLDC.
- Adjust the speed by **freemaster**
- Control by trapezoidal wave algorithm

## Configurations

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.

- Board settings refer to the development board documentation [ PWM_PIN ](lab_board_motor_ctrl_pin) related content

- Click [DRV-LV50A-MP1907 Motor Driver Board](lab_drv_lv50a_mp1907) section and configure

- **freemaster** settings refer to [freemaster configuration](lab_samples_freemaster_configure) section
  - **freemaster** project file address `./motor_block.pmpx`

```{note}

This code does not support any compilation optimization, please turn off compilation optimization

```

## Running the Demo

The motor runs at a speed of 20r/s.
The following data can be configured by clicking on the GO icon of **freemaster**：

``PID_I`` float

``PID_P`` float

``set_speed`` float, range +40~-40, unit r/s

也可以查看如下参数：

``speed`` float, unit r/s

```{warning}
Pay attention to the current size, when abnormalities occur, please cut off the power at any time
```
