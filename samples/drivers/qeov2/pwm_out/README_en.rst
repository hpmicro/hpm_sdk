.. _qeov2_pwm_out:

QEOV2_PWM_OUT
==========================

Overview
--------

QEO_PWM_OUT sample shows QEO generates PWM signals through PWM peripheral according to received postion.

Board Setting
-------------

- Output PWM signals at the specified pin. （Please refer to   :ref:`Pin Description <board_resource>` ）

Running the example
-------------------

When the project runs correctly, the serial port terminal will receive the following information, use oscilloscope/logic analyzer to observe the signal on the pin:

.. code-block:: console

   QEO PWM example
   QEO generate PWM signal with software inject postion
   QEO generate PWM signal with hardware provide postion

The logic analyzer to observe the PWM signals on the pin


.. image:: doc/qeo_pwm_1.png
   :alt:


.. image:: doc/qeo_pwm_2.png
   :alt:
