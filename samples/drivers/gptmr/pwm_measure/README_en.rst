.. _gptmr_measure_pwm:

GPTMR measure PWM
==================================

Overview
--------

The example shows measure PWM wave.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
Connect external PWM signal to GPTMR capture pin.

Running the example
-------------------

Running the example,if there is a PWM wave input, the serial terminal outputs the following information,which are the frequency and duty cycle of the PWM wave.

.. code-block:: console

   gptmr pwm measure test
   gptmr pwm measure test
   [table_0]measured frequency: 1000.049987
   [table_0]measured duty cycle: 50.00%
   [table_1]measured frequency: 1000.049987
   [table_1]measured duty cycle: 50.00%
   [table_2]measured frequency: 1000.049987
   [table_2]measured duty cycle: 50.00%
   [table_3]measured frequency: 1000.049987
   [table_3]measured duty cycle: 50.00%
   [table_4]measured frequency: 1000.049987
   [table_4]measured duty cycle: 50.00%
   [table_5]measured frequency: 1000.049987
   [table_5]measured duty cycle: 50.00%
   [table_6]measured frequency: 1000.049987
   [table_6]measured duty cycle: 50.00%
   [table_7]measured frequency: 1000.049987
   [table_7]measured duty cycle: 50.00%
   [table_8]measured frequency: 1000.049987
   [table_8]measured duty cycle: 50.00%
   [table_9]measured frequency: 1000.059997
   [table_9]measured duty cycle: 50.00%

