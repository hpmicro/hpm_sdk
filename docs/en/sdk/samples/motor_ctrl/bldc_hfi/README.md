# BLDC Square wave inject(HFI)

## Overview

The **bldc_hfi** project contains the sensorless speed control of DC brushless motors.
- Use the **FOC** control algorithm
- Sensorless control algorithm for **HFI**
- Senseless low-speed performance

## Configurations

- This program uses the **57B3C0730** brushless motor of " ShiDaiChaoQun".

- Board settings refer to the development board documentation [Motor Pin](lab_board_motor_ctrl_pin) related content

## Code Options

- Add `sdk_compile_options("-mabi=ilp32f")` to the project's CMakeLists.txt to get a higher performance

## Running the demo

The motor will run at a low speed of 1 r/s. 

The serial console message is as follows:

```console

Motor Inject demo.

```

```{warning}

- After the motor is powered on, it will first perform the "magnetic pole identification" action. If the motor is disturbed at this time, it will produce a jitter.

- Pay attention to the current size, when abnormalities occur, please cut off the power at any time

```
