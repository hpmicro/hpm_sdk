.. _ffa:

FFA
======

Overview
------------
This example demonstrates three main functions of HPM FFA:
- Fast Fourier Transform (FFT)
- Inverse Fast Fourier Transform (IFFT)
- Finite Impulse Response (FIR) filtering

Working Principle
-----------------
This example demonstrates FFA functionality through three test cases:

1. FFT Test
   - Supports complex FFT with various points (16/32/64/256/512 points)
   - Uses Q31 format complex input data
   - Verifies results by comparing with golden data

2. IFFT Test
   - Supports complex IFFT with various points (8/16/32 points)
   - Uses Q31 format complex input data
   - Verifies results by comparing with golden data

3. FIR Filtering Test
   - Uses predefined filter coefficients
   - Performs real FIR filtering on input data
   - Verifies filtering results by comparing with golden data

Hardware Requirements
---------------------
- Board with FFA peripheral

Running the example
-------------------
- UART terminal displays test execution results
- Shows whether each test passed or failed (PASSED/FAILED)

