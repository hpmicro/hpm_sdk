.. _fix_duty_cycle:

Fix Duty Cycle
============================

Overiew
-------

PWM supports the function of fixing the duty cycle and modifying the frequency. This project demonstrates the function of modifying the frequency while ensuring that the duty cycle of the waveform remains unchanged through two methods: hardware association or software calculation.

Configurations
--------------

- Oscilloscope

- Install the serial terminal, view  :ref:`board information <board_resource>` , and configure the serial terminal parameters

-  :ref:`PWM_P0 pin <board_resource>`  Check the information according to the board model

Running the Demo
----------------

- After power-on, it can be observed through an oscilloscope that the frequency of the waveform output from the P0 pin is constantly changing while the duty cycle remains unchanged

- Serial port printing information:


.. code-block:: console

       pwmv2 fix duty cycle example


       >> Generate fix duty cycle by calc
       Two waveforms will be generated, PWM P0 is the target waveform


       >> Generate fix duty cycle by shadow
       Two waveforms will be generated, PWM P0 is the target waveform
       test done



- waveform diagrams:

    .. image:: ../doc/fix_dc_1.png
       :alt: 波形图

    .. image:: ../doc/fix_dc_2.png
       :alt: 波形图

    .. image:: ../doc/fix_dc_3.png
       :alt: 波形图


.. note::

   Output the PWM waveform once, and if you observe the waveform again, you need to run the program again.
