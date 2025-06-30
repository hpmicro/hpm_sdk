.. _dsp:

DSP
======

Overview
--------

The dsp_demo sample project demonstrates the DSP CFFT (Complex Fast Fourier Transform) and CIFFT (Complex Inverse Fast Fourier Transform) calculations. It performs a 32-point FFT transform and validates the results through detailed error analysis.

The example includes:
- Complex FFT (CFFT) calculation using radix-2 algorithm
- Complex IFFT (CIFFT) calculation using radix-2 algorithm
- Input signal generation and scaling
- Detailed error analysis including MAE, RMSD, NRMSD, and SNR metrics
- Result validation against golden reference data

Board Setting
-------------

No special settings are required.

Running the example
-------------------

When the project runs correctly, the serial port terminal will output the following information:

.. code-block:: console

   --------------------
   after CFFT_RD2, maxdiff= 0x0080 [0.00390816]
   --------------------
   MAE is 0.00079375, RMSD is 0.00176055, NRMSD is 0.00005502, MAXDIFF is 0.00390816, SNR is 67.12823486
   CFFT_RD2 out scaleup by 64
   after CIFFT_RD2, maxdiff= 0x0009 [0.00029564]
   --------------------
   MAE is 0.00012192, RMSD is 0.00014900, NRMSD is 0.00007450, MAXDIFF is 0.00029564, SNR is 73.52618408
   CIFFT_RD2 out scaleup by 2
   CIFFT_RD2 PASS

The output shows:
- The maximum difference between calculated and golden results
- Mean Absolute Error (MAE)
- Root Mean Square Deviation (RMSD)
- Normalized Root Mean Square Deviation (NRMSD)
- Maximum Difference (MAXDIFF)
- Signal-to-Noise Ratio (SNR)
- Scaling factors used in the calculations

The test is considered successful if:
- For CFFT: NRMSD is less than the threshold (0.00007 for normal compilation, 0.0002 for zcc)
- For CIFFT: NRMSD is less than the threshold (0.0003 for normal compilation, 0.005 for zcc)

