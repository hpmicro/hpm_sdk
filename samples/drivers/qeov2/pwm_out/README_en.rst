.. _qeov2_pwm_out:

QEOV2_PWM_OUT
==========================

Overview
--------

The QEO_PWM_OUT sample project demonstrates the functionality of QEOv2 (Quadrature Encoder Output Version 2) peripheral in generating PWM waveforms based on position information. QEOv2 can receive position information either through software injection or from hardware (such as MTG\SEI), and generate PWM signals based on preset resolution lines.

Working Process
----------------

This example demonstrates two working modes:

1. Software Position Injection Mode
   Configuration steps:

   - Initialize QEO module and configure PWM output mode
   - Configure PWM signal generation parameters:

     * Set resolution lines (4 lines)
     * Configure PWM force output table
     * Enable PWM signal output

   - Software position injection process:

     * Calculate position increment value
     * Enable software position injection
     * Cyclically inject position values for rotation
     * Wait for PWM signal generation
     * Disable position injection

2. Hardware Position Input Mode

   In this example, the hardware generator provides position information to QEO, which generates PWM signals in real-time based on the received position information.

   Configuration steps:

   - Initialize QEO module with the same PWM parameters as software mode
   - Configure hardware position input:

     * Configure hardware position generator
     * Configure PWM safety mode table
     * Enable PWM signal output

   - Enable motor system timestamp function

Hardware Setting
-----------------

Use oscilloscope/logic analyzer to observe the PWM waveforms on specified PWM output pins (Please refer to :ref:`Pin Description <board_resource>` section).

For MCU that does not support MTG peripheral, a QEI peripheral connected to an ABZ encoder (4000 lines) is required to provide hardware position input for testing.

Running the Example
--------------------

When the project runs correctly, you will observe the following:

1. Serial terminal output:

.. code-block:: console

   QEO PWM example
   QEO generate PWM signal with software inject position
   QEO generate PWM signal with hardware provide position

2. PWM Signal Output:

Using oscilloscope/logic analyzer, you can observe two types of PWM waveforms:
- PWM waveform in software position injection mode:

.. image:: doc/qeo_pwm_1.png
   :alt: PWM Waveform in Software Injection Mode

- PWM waveform in hardware position input mode:

.. image:: doc/qeo_pwm_2.png
   :alt: PWM Waveform in Hardware Input Mode
