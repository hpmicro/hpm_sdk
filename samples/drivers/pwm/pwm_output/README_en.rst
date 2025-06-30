.. _pwm_output:

PWM Output
====================

Overview
--------

This project demonstrates how to configure and output several different types of PWM waveforms through MCU pins.

The demo includes the following PWM output types:

- Forced output (high/low level)
- Edge-aligned PWM
- Edge-aligned PWM with fault mode
- Center-aligned PWM
- Center-aligned complementary PWM
- Edge-aligned PWM with jitter

Hardware Requirements
---------------------

- Dual Channel Oscilloscope
- Serial terminal for debugging output
- Connect PWM output pins according to your board model (refer to :ref:`board information <board_resource>`)

Configuration
-------------

- Install the serial terminal and configure it according to :ref:`board information <board_resource>`
- Connect PWM output pins (PWM_P0 and PWM_P1) as specified in :ref:`board resource <board_resource>`
- Connect oscilloscope probes to PWM output pins

Running the Demo
----------------

After power on, the oscilloscope will display the following waveforms in sequence:

1. Forced output:
   - Both P0 and P1 output high level for 5 seconds
   - Both P0 and P1 output low level for 5 seconds

2. Edge-aligned PWM:
   - P0: Target waveform with duty cycle varying from 0% to 100% and back
   - P1: Reference waveform with 50% duty cycle

3. Edge-aligned PWM with fault mode:
   - Similar to edge-aligned PWM but with fault protection enabled
   - Demonstrates PWM behavior under fault conditions

4. Center-aligned PWM:
   - P0: Target waveform with duty cycle varying from 0% to 100% and back
   - P1: Reference waveform with 50% duty cycle

5. Center-aligned complementary PWM:
   - P0 and P1: Complementary waveforms with duty cycle varying from 0% to 100% and back
   - Includes dead time between complementary signals

6. Edge-aligned PWM with jitter:
   - P0: Target waveform with duty cycle varying from 0% to 100% and back
   - P1: Reference waveform with 50% duty cycle
   - Includes jitter effect for demonstration

Serial port output will show the progress of each test:

.. code-block:: console

   pwm example

   >> Test force PWM output on P0 and P1
   Output high
   Output low

   >> Generate edge aligned waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate edge aligned waveform and fault mode enable
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate central aligned waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate central aligned waveform in pair
   Two waveforms will be generated in pair, PWM P0 and P1 are target
   waveforms whose duty cycle will be updated from 0 - 100 and back to 0

   >> Generate edge aligned jit waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   test done

.. note::

   The demo runs through all PWM output types once. To observe the waveforms again, you need to reset and run the program again.

