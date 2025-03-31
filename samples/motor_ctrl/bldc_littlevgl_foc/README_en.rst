.. _touch_screen_control_of_bldc_bldc_lvgl:

Touch screen control of BLDC (BLDC LVGL)
================================================================================

Overview
--------

**bldc_littlevgl_foc**  control of DC brushless motor by operating the touch screen.
- Use the **FOC**  control algorithm

- Use the **littlevgl**  motor drive library

- Touch screen operation to control motor position and speed

- Real-time display of motor speed and position

Configurations
--------------

- This program uses the **BLM57050-1000**  brushless motor of "Leisai Intelligence", please refer to the `Leisai Intelligence <https://leisai.com/>`_ website for the specific parameters of the motor.

- Board settings refer to the development board documentation  :ref:`Motor Pin <board_resource>`  related content

Running the demo
----------------

After the initialization operation, the screen will display the interface, click on the screen for speed and position control, and the screen will show the speed and position information.


.. warning::

   - After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter.

   - Pay attention to the current size, when abnormalities occur, please cut off the power at any time



.. note::

   - Dependency on lvgl version 8.3.5

