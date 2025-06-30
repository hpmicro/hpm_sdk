Stepper Motor Closed Loop Control Example
=============================================

Overview
--------

This demo demonstrates closed-loop control of a stepper motor:
- FOC (Field Oriented Control) algorithm
- Speed and position closed-loop control
- Current loop control
- Support for switching between speed mode and position mode

Configurations
--------------

- 42 Stepper Motor, 24v, 50 Pair of Poles
- Stepper motor driver board **24V_STEP_NONE_2A**
    - See :ref:`24v_step_none_2a_revb` for hardware connection details

Running the demo
----------------

The program supports two operating modes:

1. Position Mode (Enter 0)
   - Input target position value via serial port
   - Motor will rotate to and maintain the specified position

2. Speed Mode (Enter 1)
   - Input target speed value via serial port (Range: -10 ~ 10 r/s)
   - Motor will continuously run at the specified speed

The serial console message example:

.. code-block:: console

   step motor demo.
   loop current tick: xxx
   Mode selection:
   0. Location mode.
   1. Speed mode.
   Enter mode code:

   // Speed mode example:
   Speed mode, motor run, speed is: 1.000000.
   Input speed:
   2
   loop current tick: xxx
   Speed mode, motor run, speed is: 2.000000.
   Input speed:

   // Position mode example:
   Location mode, motor run, The location is: 0.
   Input Location:
   1000
   loop current tick: xxx
   Location mode, motor run, The location is: 1000.
   Input Location:

.. warning::

   - Pay attention to the motor current, cut off power immediately if abnormal
   - Monitor the temperature of the stepper motor during long-term operation

