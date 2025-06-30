RFFT Example
============

Overview
--------

This example demonstrates how to use the Real Fast Fourier Transform (RFFT) functionality in HPM SDK. The example generates a composite signal containing three sine waves with different frequencies and amplitudes, performs RFFT transformation, and verifies the results.

Features
--------

- Generate composite signal with three sine waves
- Perform real fast Fourier transform
- Calculate magnitude spectrum of transformed results
- Verify frequency and amplitude accuracy
- Test RFFT performance with different points (64-1024 points)

Hardware Requirements
---------------------

- HPM6750EVKMINI development board
- HPM6200EVK development board
- HPM5300-FPGA development board
- HPM6800-VALIDATION development board

Software Requirements
---------------------

- HPM SDK
- CMake 3.20.0 or higher
- Cross-compilation toolchain

Build Instructions
------------------

1. Enter the example directory
2. Create and enter build directory
3. Run CMake configuration
4. Build the project

Run Instructions
----------------

1. Flash the compiled binary to the development board
2. View output through serial port
3. Observe RFFT transformation results and performance data

Output Description
------------------

The program will output the following information:
- CPU frequency
- Execution time for different point RFFT
- Amplitude of frequency components
- Test results (PASS/FAIL)

Programming
-----------

The software generates several sine waves with different amplitudes and phases and superimposes them to generate a waveform. The RFFT transform is then used to find the amplitude and frequency characteristics. The sine waves are shown below:

- Amplitude 11, frequency 8
- Amplitude 19, frequency 17
- Amplitude 13, frequency 24

Performance Optimization
------------------------

- segger turns on O3 optimization by default

Running the example
-------------------

When the project runs correctly, the serial terminal outputs the number of samples, the amplitude and frequency characteristics data, and the running time of each unit, and outputs PASS or ERROR NUM at the end.

The serial port output is shown below:

.. code-block:: console

   Rfft demo: 64.
   total times:5133 tick.
   ------------------------------------
   DC component amplitude:5.000004.
   Frequency:8.000000, Amplitude:11.000010.
   Frequency:17.000000, Amplitude:18.999992.
   Frequency:24.000000, Amplitude:13.000002.
   ------------------------------------
   Rfft demo: 128.
   total times:7956 tick.
   ------------------------------------
   DC component amplitude:4.999997.
   Frequency:8.000000, Amplitude:11.000002.
   Frequency:17.000000, Amplitude:18.999996.
   Frequency:24.000000, Amplitude:13.000004.
   ------------------------------------
   Rfft demo: 256.
   total times:25266 tick.
   ------------------------------------
   DC component amplitude:4.999996.
   Frequency:8.000000, Amplitude:11.000001.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:13.000004.
   ------------------------------------
   Rfft demo: 512.
   total times:37868 tick.
   ------------------------------------
   DC component amplitude:4.999998.
   Frequency:8.000000, Amplitude:11.000000.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:13.000000.
   ------------------------------------
   Rfft demo: 1024.
   total times:122305 tick.
   ------------------------------------
   DC component amplitude:4.999998.
   Frequency:8.000000, Amplitude:11.000000.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:13.000000.
   ------------------------------------
   PASS.
   ----------------END--------------------

Performance Data
----------------

The RFFT performance for different sample points is shown in the following table:

.. list-table::
   :header-rows: 1

   * - Samples
     - 64
     - 128
     - 256
     - 512
     - 1024
   * - DSP rfft
     - 5133
     - 7956
     - 25266
     - 37868
     - 122305
