# BLDC Trapezoidal Wave Control

## Overview

**Bldc_block** project demonstrates speed control of the BLDC.
- Control by trapezoidal wave algorithm

## Configurations

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.

- Board settings refer to the development board documentation [Motor Pin](lab_board_motor_ctrl_pin) related content

```{note}

This code does not support any compilation optimization, please turn off compilation optimization

```

## Running the Demo

The motor runs at a speed of 20r/s.
The following data can be configured by serial console ：

``speed`` float, range +40~-40, unit r/s.

The serial console message is as follows:

```console

Speed mode, motor run, speed is: 20.000000.
Input speed:
10.5

Speed mode, motor run, speed is: 10.500000.
Input speed:

```

```{warning}
Pay attention to the current size, when abnormalities occur, please cut off the power at any time
```
