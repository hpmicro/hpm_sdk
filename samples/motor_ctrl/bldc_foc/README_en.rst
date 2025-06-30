.. _bldc_foc_control:

BLDC FOC Control
================================

Overview
--------

The **bldc_foc** project contains the speed control and position control of DC brushless motors.
- Use the **FOC** (Field Oriented Control) control algorithm
- Support both hardware and software current loop implementation
- Support dual closed-loop control of speed and position
- Support both ABZ encoder and QEI encoder for position detection

Configurations
--------------

- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", please refer to the `Leisai Intelligence <https://leisai.com/>`_ website for the specific parameters of the motor.

- Board settings refer to the development board documentation :ref:`Motor Pin <board_resource>` related content

    - Ensure PWM frequency is set correctly
    - Ensure motor pole pairs is set correctly
    - Ensure ADC sampling configuration is correct

- Peripheral Configuration:

    - PWM Configuration:
        - Use BLDC PWM module
        - Configure dead time to prevent shoot-through
        - Configure complementary PWM output
        - Support three-phase six-channel PWM output
        - Support PWM synchronized ADC sampling trigger
        - Use shadow register to update PWM duty cycle
        - Support PWM fault protection

    - ADC Configuration:
        - Use two ADC modules to sample U-phase and V-phase current respectively
        - Configure ADC reference voltage
        - Configure op-amp gain
        - Configure sampling precision
        - Configure sampling resolution
        - Support DMA transfer
        - Support ADC synchronized sampling
        - Support ADC trigger source configuration
        - Support ADC sampling interrupt

    - Encoder Configuration:
        - Support ABZ encoder and QEI encoder
        - Configure encoder precision
        - Configure encoder direction
        - Configure encoder sampling period
        - Support encoder position counting
        - Support encoder speed calculation

    - Timer Configuration:
        - Use GPTMR as timer
        - Configure current loop sampling period
        - Configure speed loop sampling period
        - Configure position loop sampling period
        - Support timer interrupt

Current Loop Time
-----------------

Hardware current loop will show time zero. The current loop calculation time is around 1us, this time fluctuates depending on the angle and fluctuates in the range of 25%, which is reproduced by performing the following operation:

- Turning off the `mcl_app_config.h` macros except for `MCL_EN_LOOP_TIME_COUNT`
- Compile option `flash_xip_release`

Running the demo
----------------

The motor runs at default speed.
The following data can be configured by serial console:

`speed` float, range +40~-40, unit r/s
- Positive value for forward rotation
- Negative value for reverse rotation
- 0 for stop

`pos` int, (-4000, +4000) corresponds to (-360, +360) degrees
- Positive value for clockwise direction
- Negative value for counterclockwise direction

`mode` bool, 1- Speed mode 0- Position mode

- Speed mode
    - Enter the correct speed parameters with serial console
    - Use PID algorithm for speed control

- Position mode
    - Set the position of the motor's shaft, then the motor's shaft will be locked at the specified position
    - Use PID algorithm for position control

.. code-block:: console

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

.. warning::

   - After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter
   - Pay attention to the current size, when abnormalities occur, please cut off the power at any time
   - When entering values for speed and position, a line break is required as an end-of-entry flag
   - For first-time operation, it is recommended to start testing at low speed
   - In position mode, ensure the motor is not subject to external force interference

