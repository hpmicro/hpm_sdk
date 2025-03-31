.. _bldc_offline_parameter_detection_example:

BLDC Offline Parameter Detection Example
================================================================================

Overview
--------

This project demonstrates an offline method for detecting parameters of a brushless DC (BLDC) motor. The detected parameters include:
- Flux linkage

- Direct-axis inductance (Ld)

- Quadrature-axis inductance (Lq)

- Phase resistance (Rs)

- Phase inductance (Ls)

design theory
-------------

`Please refer to the link <https://kb.hpmicro.com/2024/08/07/%e7%94%b5%e6%9c%ba%e7%a6%bb%e7%ba%bf%e5%8f%82%e6%95%b0%e8%be%a8%e8%af%86/>`_

Configuration
-------------

- This example uses a Leisai Intelligent BLM57050-1000 BLDC motor. For specific motor parameters, please refer to the `Leisai Intelligent website <https://leisai.com/>`_.

- Board settings are based on the documentation for the  :ref:`Motor Pin <board_resource>`  section of the development board.

- After completing the above steps, power up the drive board and observe that there are no abnormal currents. Then power up the core board and again confirm that there are no abnormal currents. You can then flash the program and observe its operation.

Operation Phenomenon
--------------------

When the project runs correctly, the following output is displayed via the serial console:

.. code-block:: console

   motor parameter detection demo.
   flux: 0.044300, ld: 0.000968, lq: 0.001808, ls: 0.002777, rs: 1.037876
   flux: 0.044056, ld: 0.000992, lq: 0.001803, ls: 0.002795, rs: 1.022872
   flux: 0.044366, ld: 0.000987, lq: 0.001785, ls: 0.002773, rs: 1.040307
   flux: 0.043980, ld: 0.000990, lq: 0.001783, ls: 0.002773, rs: 1.028208
   flux: 0.046708, ld: 0.000984, lq: 0.001801, ls: 0.002785, rs: 1.097037
   flux: 0.043836, ld: 0.000989, lq: 0.001783, ls: 0.002771, rs: 1.020653
   flux: 0.045053, ld: 0.000988, lq: 0.001786, ls: 0.002774, rs: 1.059921
   flux: 0.044345, ld: 0.000989, lq: 0.001783, ls: 0.002772, rs: 1.049591



.. warning::

   - After the motor is powered on, it will first perform the "alignment" action. If the motor is disturbed at this time, it will produce a jitter.

   - Pay attention to the current size, when abnormalities occur, please cut off the power at any time

