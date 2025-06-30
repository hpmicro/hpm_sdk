.. _qeov2_sei2abz:

QEOV2_SEI2ABZ
==========================

Overview
--------

The QEOV2_SEI2ABZ sample project demonstrates how QEOv2 (Quadrature Encoder Output Version 2) peripheral acquires position information through SEI interface (using Tamagawa sensor TS5700N8401), and dynamically adjusts the width of QEO ABZ signal output based on the acquired position information to achieve uniform ABZ signal output.

Working Process
----------------

This example demonstrates the conversion process from SEI position information to QEO ABZ signals:

1. SEI Position Acquisition:

   - Configure SEI interface sampling parameters
   - Enable SEI sampling interrupt
   - Acquire sensor position information in real-time

2. QEO ABZ Signal Generation:

   - Configure ABZ signal parameters:

     * Set encoder resolution (4096 lines/revolution)
     * Set maximum output frequency (250kHz)
     * Configure position synchronization function

   - Dynamic Width Adjustment:

     * Calculate position change in SEI sampling interrupt
     * Adjust ABZ signal width based on position change
     * Ensure uniform ABZ signal output

Hardware Setting
----------------

1. Sensor Connection:

   - Connect Tamagawa sensor TS5700N8401's DATA_P/DATA_N signals to corresponding board pins
   - Provide 5V power supply to the sensor

2. Signal Observation:

   - Connect board's ABZ output pins (QEO_A/QEO_B/QEO_Z) to logic analyzer
   - For capturing output waveforms (Please refer to :ref:`Pin Description <board_resource>` section)

Running the Example
-------------------

When the project runs correctly, you will observe the following:

1. Serial terminal output:

   .. code-block:: console

      QEO SEI2ABZ example
      QEO generate ABZ signal with hardware(SEI) provide postion
      device init finish

2. ABZ Signal Output:

   When rotating the sensor, you can observe through logic analyzer:

   - A/B signals: Quadrature square waves with 90-degree phase difference
   - Z signal: One reference pulse per revolution
   - Signal characteristics: Uniform ABZ waveforms matching the rotation speed

