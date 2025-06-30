.. _qeo_dac_out:

QEO_DAC_OUT
======================

Overview
---------

The QEO_DAC_OUT sample project demonstrates the functionality of QEO (Quadrature Encoder Output) peripheral in generating waveform signals based on position information. QEO can receive position information either through software injection or from hardware (such as MMC), and generate corresponding waveforms (such as sine wave, cosine wave, etc.) based on preset wave type and resolution settings, which can be output as analog signals through DAC.

Working Process
----------------

This example demonstrates two working modes:

1. Software Position Injection Mode
   Configuration steps:

   - Initialize QEO module and configure wave output mode
   - Configure wave generation parameters:

     * Set wave type (e.g., cosine wave)
     * Set resolution lines to determine wave period
     * Configure three-phase wave phase shifts (120° and 240°)

   - Software position injection process:

     * Calculate position increment value
     * Enable software position injection
     * Cyclically inject position values to generate wave samples
     * Wait for wave calculation completion
     * Get and store wave output values
     * Disable software position injection

2. Hardware Position Input Mode

   In this example, the MMC (Motor Motion Control) peripheral provides position information to QEO, which generates wave outputs in real-time based on the received position information.

   Configuration steps:

   - Initialize QEO module with the same wave parameters as software mode
   - Configure hardware position input:

     * Connect MMC position output to QEO through TRGM
     * Configure MMC open-loop prediction mode

   - Enable motor timestamp function for synchronization

Hardware Setting
-----------------

Observe the output voltage waveforms on the specified DAC pins (Please refer to :ref:`Pin Description <board_resource>` section).

Notes
-----------

1. QEO supports outputting 3 wave signals, but the actual observable waveforms are limited by the available DAC channels on the development board
2. Wave output value range is 0-65535, corresponding to DAC full-scale output
3. In software injection mode, position value range is 0-0x100000000, evenly divided according to resolution lines

Running the Example
--------------------

When the project runs correctly, you will observe the following:

1. Serial terminal output:

.. code-block:: console

   QEO DAC wave example
   QEO generate wave with software inject position
   qeo wave0 output:
   65535
   65526
   65496
   // ... more data points ...
   QEO generate wave base on hardware(MMC) provide position

2. Waveform output:

- Using tools like Excel, you can plot the serial output data to visualize the phase relationship of three-phase waveforms:

.. image:: doc/qeo_dac_1.png
   :alt: Three-phase waveform data plot

- Using oscilloscope to observe actual output waveforms on DAC pins:

.. image:: doc/qeo_dac_2.png
   :alt: DAC output waveform display
