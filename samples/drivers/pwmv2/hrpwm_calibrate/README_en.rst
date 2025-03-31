.. _hrpwm_calibration:

HRPWM Calibration
==================================

Overiew
-------

**pwm**  project demo how to calibrate the effect of temperature in the use of high-precision pwm.

Configurations
--------------

- Dual Channel Oscilloscope

- Install the serial terminal, view  :ref:`board information <board_resource>` , and configure the serial terminal parameters

-  :ref:`PWM_P0 and PWM_P1 pins <board_resource>`  Check the information according to the board model

Running the Demo
----------------

- After power on, through the oscilloscope can be observed in turn P0, P1 output duty cycle of 50 percent, of which the high-precision part of the P0 to 0-255 cents of a clock cycle change. When the chip temperature changes, the program will be calibrated and printed through the serial port to complete or fail

- Serial port printing information:


.. code-block:: console

   pwm example


   >> Generate edge aligned waveform
   Two waveforms will be generated, PWM P1 is the target waveform
   whose hrpwm cycle will be updated from 0 - 255 and back to 0; PWM P2 is a reference

   calibration done

