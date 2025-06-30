.. _pwmv2_output:

Pwm Output
====================

Overview
--------

**pwm** project configures several different types of waveforms and outputs them.

- Forced output
- Edge-aligned PWM
- Center-aligned PWM
- Center-aligned complementary PWM
- PWM fault protection
- Four comparator output with different logic modes (AND, XOR, CD)
- PWM DAC output
- Edge-aligned PWM with jitter compensation

Configurations
--------------

- Dual Channel Oscilloscope
- Install the serial terminal, view :ref:`board information <board_resource>`, and configure the serial terminal parameters
- :ref:`PWM_P0 and PWM_P1 pins <board_resource>` Check the information according to the board model

Running the Demo
----------------

- After power on, the oscilloscope can observe:
  - P0, P1 output high level and low level
  - Edge-aligned PWM with duty cycle varying from 0% to 100%
  - Center-aligned PWM with duty cycle varying from 0% to 100%
  - Center-aligned complementary PWM with duty cycle varying from 0% to 100%
  - Four comparator output with different logic modes
  - PWM DAC output with varying duty cycle
  - Edge-aligned PWM with jitter compensation

- After the fault pin is given a high level, P0 and P1 pin PWM outputs are low, and after the fault pin is given a low level, P0 and P1 pin PWM return to normal

- Serial port printing information:

.. code-block:: console

   pwm example

   >> Test force PWM output on P0 and P1
   Output high
   Output low

   >> Generate edge aligned waveform
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

   >> Generate four cmp waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate pwm dac waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   test done

.. note::

   The demo outputs each PWM waveform once in sequence. To observe the waveforms again, you need to run the program again.

