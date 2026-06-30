.. _ffa:

FFA
======

Overview
------------
This example demonstrates main functions of HPM FFA:

- Fast Fourier Transform (FFT)
- Inverse Fast Fourier Transform (IFFT)
- Finite Impulse Response (FIR) filtering
- Chained Command Mode: link multiple FFT/IFFT/FIR operations by a descriptor list and let hardware execute them sequentially

Working Principle
-----------------
This example demonstrates FFA functionality through the following test cases:

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

4. Chained FIR Test
   - Links multiple FIR operations by chained command mode
   - Hardware automatically loads and runs the next descriptor after the previous operation is done

5. Chained FFT Test
   - Links multiple FFT operations by chained command mode

6. Chained Mixed Test
   - Links FFT and FIR operations by chained command mode (FFT + FIR)

Hardware Requirements
---------------------
- Board with FFA peripheral

Hardware Setting
----------------

No special settings.

Running the example
-------------------
When the example runs successfully, the UART terminal prints a menu similar to the following:

.. code-block:: console

   *******************************************************************************
   *                                                                             *
   *    FFA Test demo                                                            *
   *    1. Complex Q31 FFT Test                                                  *
   *    2. 8-Point Complex Q31 IFFT Test                                         *
   *    3. Real Q31 FIR Test                                                     *
   *    4. Chained FIR Test (3 FIR operations)                                   *
   *    5. Chained FFT Test (2 FFT operations)                                   *
   *    6. Chained Mixed Test (FFT + FIR)                                        *
   *                                                                             *
   *******************************************************************************

Notes
-----
- In chained mode, a descriptor contains 10 consecutive 32-bit fields
- In chained mode, hardware loads and overwrites corresponding length registers from the descriptor, so every field in the descriptor must be filled correctly
- **Alignment requirements**:

  - **The lowest 2 bits of NXT_ADDR must be 0** (4-byte aligned).
  - If DCache is enabled, it is recommended to align the descriptor array to **cache line (e.g. 64 bytes)**, flush DCache for descriptors and input buffers before starting chained mode, and invalidate DCache for output buffers to avoid stale data.
