# BLDC SMC

## Overview

The **bldc_smc** project contains the sensorless speed control of DC brushless motors.
- Use the **FOC** control algorithm
- Sensorless control algorithm for **SMC**

## Configurations

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", please refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.

- Board settings refer to the development board documentation [Motor Pin](lab_board_motor_ctrl_pin) related content

## Code Options

- Add `sdk_compile_options("-mabi=ilp32f")` to the project's CMakeLists.txt to get a higher performance

```{note}

This code does not support any compilation optimization

```

## Running the demo

The motor accelerates at speeds from 15 to 40 revolutions per second when the project is running correctly, then decelerates from 40 to 15 revolutions per second and keeps cycling.

The serial console message is as follows:

```console

bldc smc demo.

```

```{warning}

- After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter.

- Pay attention to the current size, when abnormalities occur, please cut off the power at any time

```
