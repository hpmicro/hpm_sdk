.. _plb_pulse_output:

Pulse Output
========================

Overview
--------

This example demonstrates how to configure PLB as a 32-bit pulse generator. Main features include:

- Configure PWM to generate base clock signal
- Route signals through TRGM module
- Use PLB to generate specified number of pulse outputs

Configuration
-------------

1. PWM Configuration
   - Configure PWM to generate base clock
   - Set PWM compare value to generate square wave with specified duty cycle

2. TRGM Configuration
   - Configure trigger signal routing from PWM to PLB
   - Configure trigger signal routing from PLB to IO
   - Configure signal polarity and trigger modes

3. PLB Configuration
   - Initialize counter
   - Set counter value for generating specified number of pulses
   - Configure lookup tables (LUT) for pulse counting and output control

Board Setting
-------------

- Connect the :ref:`pulse output pin <board_resource>` to the oscilloscope

Running the example
-------------------

When the project runs correctly:

- The oscilloscope can capture continuous pulses
- The pulse period is determined by PWM_PERIOD_IN_MS macro
- The serial port terminal will output the following information:

.. code-block:: console

   PLB DEMO
