.. _rgb_led:

RGB LED
==============

Overview
--------

This example demonstrates how to control an RGB LED using PWM to achieve a breathing light effect.

Board Settings
--------------

None

Implementation
--------------

- Control the R, G, and B colors of the RGB LED using three PWM outputs
- For each channel, adjust the PWM duty cycle, gradually increasing from 0% to 100%, then gradually decreasing back to 0%, repeating this process to create a breathing light effect
- Change the shadow register update method: using another cmp match event as a hardware event to trigger PWM updates, achieving smooth PWM transitions

Running the Demo
----------------

The onboard RGB LED will cycle through the following sequence:
1. Red LED breathing effect
2. Green LED breathing effect
3. Blue LED breathing effect
4. Repeat the sequence

The breathing effect creates a smooth transition between minimum and maximum brightness for each color.