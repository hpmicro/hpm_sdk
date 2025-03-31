.. _gptmr_input_captrue:

GPTMR input captrue
======================================

Overview
--------

The example shows input capture.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
Connect external PWM signal to GPTMR capture pin.

Running the example
-------------------

Running the example, the serial terminal outputs the following information,and prints out the edge interval time captured each time,that is, the time of one cycle.

.. code-block:: console

   gptmr input capture test
   capture the pulse width : 100001 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick

