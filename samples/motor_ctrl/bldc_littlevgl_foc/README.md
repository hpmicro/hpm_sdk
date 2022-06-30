# Touch screen control of BLDC

## Overview

**bldc_littlevgl_foc** control of DC brushless motor by operating the touch screen.
- Use the **FOC** control algorithm
- Use the **littlevgl** motor drive library
- Touch screen operation to control motor position and speed
- Real-time display of motor speed and position

## Configurations

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", please refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.

- Board settings refer to the development board documentation [ PWM_PIN ](lab_board_motor_ctrl_pin) related content
- Click [HPM6750EVKMINI-TO-MOTOR Extension Board ](lab_board_hpm6750evkmini-to-motor-220530RevA) section and configure, list of supported development boards:
  - hpm6750evkmini
- Click [DRV-LV50A-MP1907 Motor Driver Board ](lab_drv_lv50a_mp1907) section and configure, list of supported development boards:
  - hpm6750evk
  - hpm6360evk
## Code Options

- Add `sdk_compile_options("-mabi=ilp32f")` to the project's CMakeLists.txt to get a higher performance

```{note}

This code does not support any compilation optimization, please turn off compilation optimization

```

## Running the demo

After the initialization operation, the screen will display the interface, click on the screen for speed and position control, and the screen will show the speed and position information.

```{warning}

- After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter.

- Pay attention to the current size, when abnormalities occur, please cut off the power at any time

```