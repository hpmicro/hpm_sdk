.. _the_phase_shifting_of_pwm:

The phase shifting of PWM
==================================================

Overiew
-------

PWM supports the function of phase-shifting. This example demonstrates the operation of phase-shifting the PWM wave upon receiving an external trigger input

Configurations
--------------

- Oscilloscope

- Install the serial terminal, view  :ref:`board information <board_resource>` , and configure the serial terminal parameters

-  :ref:`PWM_P0, PWM_P2 pins <board_resource>`  Check the information according to the board model

Running the Demo
----------------

- After power on, P0 and P2 output waveforms with the same phase. Every 3 seconds, the phase of P2 will shift by 10 degrees. The first phase change results in a 60 degree difference.

- Serial port printing information:


.. code-block:: console

       pwmv2 two pwm submodule phase sync example
       choose PWM output channel [P0 P2]


       >> P0 and P2 generate same phase firstly
       Two waveforms will be generated
       Please press trigger pin, P2 phase will shift
       P0 is a reference

       P0 and P2 now have a phase difference of 60 degrees

       P0 and P2 now have a phase difference of 70 degrees

       P0 and P2 now have a phase difference of 80 degrees

       P0 and P2 now have a phase difference of 90 degrees

       P0 and P2 now have a phase difference of 100 degrees
       ...

