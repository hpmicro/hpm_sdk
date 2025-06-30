.. _qeo_abz_out:

QEO_ABZ_OUT
======================

Overview
------------

The QEO_ABZ_OUT sample project demonstrates the functionality of QEO (Quadrature Encoder Output) peripheral in generating ABZ quadrature encoder signals based on position information. QEO can receive position information either through software injection or from hardware (such as MMC), and generate standard ABZ quadrature encoder signals based on preset resolution lines, supporting position synchronization for ABZ signal output.

Working Process
-----------------

This example demonstrates two working modes:

1. Software Position Injection Mode
   Configuration steps:

   - Initialize QEO module and configure ABZ signal output mode
   - Configure ABZ signal generation parameters:

     * Set encoder resolution (1024 lines/revolution)
     * Set maximum output frequency (250kHz)
     * Configure A/B signals with 4x frequency output
     * Configure Z signal to output one pulse per revolution

   - Software position injection process:

     * Calculate position increment value
     * Enable software position injection
     * Cyclically inject position values to simulate 2 forward revolutions
     * Wait for ABZ signals to reach injected position
     * Reset position to 0 and disable injection

2. Hardware Position Input Mode

   In this example, the MMC (Motor Motion Control) peripheral provides position information to QEO, which generates ABZ signals in real-time based on the received position information.

   Configuration steps:

   - Initialize QEO module with the same ABZ parameters as software mode
   - Configure hardware position input:

     * Connect MMC position output to QEO through TRGM
     * Configure MMC open-loop prediction mode

   - Enable motor system timestamp function

Hardware Setting
-----------------

Use oscilloscope/logic analyzer to observe the quadrature encoder signals on QEO ABZ output pins (Please refer to :ref:`Pin Description <board_resource>` section).

Notes
-----------

1. QEO ABZ Signal Uniformity Issues:

   - When QEO's position input frequency is greater than 2 times the output ABZ signal frequency (position difference between inputs does not exceed 1/4 line of ABZ), the output maintains uniformity
   - When QEO's position input frequency is less than 2 times the output ABZ signal frequency (position difference between inputs exceeds 1/4 line of ABZ), uniformity is maintained by dynamically adjusting the minimum width of ABZ signals

2. QEO ABZ Signal Position Synchronization Function

   - When QEO's current position and target sync position fall within the same 1/4 line, synchronization errors may occur. The current position or sync position should be adjusted to avoid the same 1/4 line, refer to the `qeo_abz_position_sync` function

Running the Example
-------------------

When the project runs correctly, you will observe the following:

1. Serial terminal output:

.. code-block:: console

   QEO ABZ example
   QEO generate ABZ signal with software inject position
   QEO generate ABZ signal with hardware(MMC) provide position

2. ABZ Signal Output:

Using oscilloscope/logic analyzer, you can observe the ABZ signal waveforms:
- A/B signals: Quadrature square waves with 90-degree phase difference
- Z signal: Index pulse once per revolution

.. image:: doc/qeo_abz.png
   :alt: ABZ Signal Waveform Display
