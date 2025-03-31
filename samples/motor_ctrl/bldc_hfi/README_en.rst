.. _bldc_square_wave_inject_hfi:

BLDC Square wave inject(HFI)
========================================================

Overview
--------

The **bldc_hfi**  project contains the sensorless speed control of DC brushless motors.
- Use the **FOC**  control algorithm

- Sensorless control algorithm for **HFI**

- Senseless low-speed performance


.. note::

   This demo is for experimental purpose only. It might have issues due to difference between hardwares.


Configurations
--------------

- This program uses the **57B3C0730**  brushless motor of " ShiDaiChaoQun".

- Board settings refer to the development board documentation  :ref:`Motor Pin <board_resource>`  related content

- Ensure that the development board power supply is sufficient, if the power supply is insufficient, remove the USB-HUB and insert multiple USB cables for power supply.

Running the demo
----------------

The motor will run at a low speed of 1 r/s.

The serial console message is as follows:


.. code-block:: console

   Motor Inject demo.



.. warning::

   - After the motor is powered on, it will first perform the "magnetic pole identification" action. If the motor is disturbed at this time, it will produce a jitter.

   - Pay attention to the current size, when abnormalities occur, please cut off the power at any time

