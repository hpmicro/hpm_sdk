.. _pla_pulse_output:

Pulse Output
========================

Overview
--------

This example demonstrates how to configure PLA as a 7bit pulse generator. By configuring PLA filters, AOI logic and flip-flops, it implements a 7-bit counter function that can generate continuous pulse output.

Board Setting
-------------

- Connect the pulse output pin :ref:`BOARD_PLA_IO_TRG_NUM <board_resource>` to the oscilloscope

Function Description
--------------------

- Configure PWM to generate reference clock signal
- Configure PLA's multi-level filters, AOI logic and D flip-flops
- Implement 7-bit counting functionality to generate continuous pulse output
- PWM period can be configured through macro PWM_PERIOD_IN_MS

Running the example
-------------------

When the project runs correctly:
- The oscilloscope can capture up to 127 pulses
- The serial port terminal will output the following information:

.. code-block:: console

   PLA DEMO

