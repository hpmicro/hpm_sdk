.. _rdc:

RDC
======

Overview
--------

The RDC (Resolver-to-Digital Converter) example project demonstrates how to obtain position information from a resolver using the RDC module. The example shows:

- ADC configuration for I/Q signal sampling
- PWM and TRGM configuration for ADC triggering
- RDC module configuration and calibration
- Angle calculation and output

Board Setting
-------------

Connect the resolver driver board to the motor interface (Please refer to :ref:`RDC Pin Description <board_resource>`), then connect the resolver to the driver board.

- Driver board 14V power supply
- RDC model **SG52-4-G12.7**
- The resolver has 4 pole pairs, so one electrical revolution corresponds to 1/4 mechanical revolution
- The initial position of the resolver needs to ensure sufficient voltage in both phases to acquire the zero-crossing position, otherwise position errors may occur during movement

Running the example
-------------------

When the example runs successfully, the serial terminal will output the calculated angle information. The program will:

1. Initialize and calibrate the RDC module
2. Continuously sample I/Q signals
3. Calculate and output the angle in degrees

The output will look like:

.. code-block:: console

    theta: 1.
    theta: 20.5.

Note: The angle value represents the mechanical angle in degrees.

