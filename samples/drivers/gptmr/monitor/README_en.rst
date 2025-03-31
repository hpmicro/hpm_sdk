.. _gptmr_monitor:

GPTMR MONITOR
==========================

Overview
--------

This sample demonstrates monitoring the periodic input signal. When the width exceeds the range (less than the minimum monitoring value and greater than the maximum monitoring value), a capture interrupt is triggered.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
Connect external PWM signal to GPTMR capture pin.

Running the example
-------------------

- Run the project and execute the monitoring function of the timer. The serial terminal outputs the following information. If the monitored periodic signal is within the range, the following information is output:


.. code-block:: console

   gptmr monitor test, sel: period
   gptmr freq: 100000000


- If the monitored periodic signal is out of range, the following information is output:


.. code-block:: console

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range


Note
----

- The monitoring value unit is the cycle time of the timer clock source

- The first power-on may cause detection errors due to the uncertainty of the detection rising edge time

- If the power-on time without input signal exceeds the maximum monitoring value or after reloading, the capture interrupt will be triggered and the out-of-range information will be printed.

- If the periodic signal is suddenly disconnected, the capture interrupt will not be triggered immediately and you need to wait for the reload time
