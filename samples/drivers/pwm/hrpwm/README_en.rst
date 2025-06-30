.. _hrpwm_output:

HRPwm Output
========================

Overview
--------

The **pwm** project demonstrates how to configure and output various PWM waveforms using the MCU's HRPWM module, including:

- Forced output mode: Control the PWM pin output high or low level by software
- Fault mode: Demonstrate PWM output protection and automatic recovery triggered by external fault signals
- Edge-aligned PWM: Output edge-aligned PWM waveform with variable duty cycle
- Center-aligned PWM: Output center-aligned PWM waveform with variable duty cycle
- Frequency-variable PWM: Demonstrate dynamic adjustment of PWM output frequency

Configurations
--------------

- Dual channel oscilloscope
- Install the serial terminal, view :ref:`board information <board_resource>` and configure the serial terminal parameters
- :ref:`HRPWM_P0 and HRPWM_P2 pins <board_resource>` Check the information according to the board model

Running the Demo
----------------

- After power on, the serial port will sequentially output prompt information for each function mode
- The oscilloscope can observe P0 and P2 pins outputting high level, low level, edge-aligned PWM, center-aligned PWM, and frequency-variable PWM waveforms in turn
- In edge-aligned and center-aligned modes, P0 is the target waveform with gradually changing duty cycle, and P2 is the reference waveform
- In fault mode, when an external fault signal is triggered, the PWM output enters protection state and automatically recovers on hardware or software events
- In frequency-variable mode, the PWM output frequency changes gradually
- Serial port printing information:

.. code-block:: console

   hr pwm example

   >> Test force HRPWM output on P0 and P2
   Output high
   Output low

   >> Generate edge aligned waveform
   Two waveforms will be generated, HRPWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100; HRPWM P2 is a reference

   >> Generate central aligned waveform
   Two waveforms will be generated, HRPWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100; HRPWM P2 is a reference

   >> Generate frequency-variable waveforms
   whose frequency will be updated; HRPWM P0 is the target waveform
   test done

.. note::

   The PWM waveform is output only once. If you need to observe the waveform again, please rerun the program.
