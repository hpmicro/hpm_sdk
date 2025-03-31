.. _gptmr_external_counter_mode:

GPTMR EXTERNAL COUNTER MODE
======================================================

Overview
--------

The sample demonstrates the GPTMR external counting mode function. The counter increments on the rising edge of the input capture signal. When the count value reaches the reload value
a reload interrupt occurs and the interrupt function flips the LED.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
Connect external PWM signal to GPTMR capture pin.

Note
----

The example uses a default reload value of 0xFFFF. For better observation of LED blinking, it suggests using an external PWM frequency above 50KHz.

Running the example
-------------------

When running the example, it performs the external counting mode of GPTMR, displaying the following message on the serial terminal and causing the onboard LED to blink


.. code-block:: console

   gptmr cnt_mode test
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
