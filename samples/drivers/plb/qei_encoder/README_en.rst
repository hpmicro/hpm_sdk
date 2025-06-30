.. _plb_qei_encoder_example:

PLB QEI Encoder Example
=======================

Overview
--------

This example demonstrates how to use the PLB peripheral to implement a Quadrature Encoder Interface (QEI). PLB can be configured as AB phase or ABZ phase encoder interface to measure motor position and revolution count.

Key features:
 - Supports both AB phase and ABZ phase encoder modes
 - Configurable pulses per revolution
 - Digital filtering for input signals
 - Position count and revolution count monitoring
 - Z phase triggered revolution counting

Board Setting
-------------

- Connect the motor board and motor to the development board
- Connect encoder signals based on encoder type:
   - AB mode: Connect encoder A and B signals to corresponding board pins
   - ABZ mode: Connect encoder A, B and Z signals to corresponding board pins

Running the example
-------------------

When the project runs correctly, the serial port terminal will output encoder position and revolution information every 100ms:

.. code-block:: console

   hase:3133. z:1
   phase:597. z:2
   phase:1792. z:2
   phase:2596. z:2
   phase:2593. z:2
   /* ... */

Notes
-----

- When phase counting is enabled, pulses per revolution must be greater than 4
- Z phase function is only valid in ABZ mode

