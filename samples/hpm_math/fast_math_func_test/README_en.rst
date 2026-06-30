.. _fast_math_func_test:

Fast Math Functions Test
========================

Overview
--------

This example demonstrates the usage and performance of the fast math functions
implemented in the HPM Math middleware. The implementation provides optimized
trigonometric and inverse trigonometric functions with configurable trade-offs
between speed and accuracy.

Supported Functions
-------------------

1. **Sin/Cos Functions** (lookup table based)

   - ``hpm_math_fast_sinf`` / ``hpm_math_fast_cosf``: No interpolation (fastest)
   - ``hpm_math_fast_sinf_linear`` / ``hpm_math_fast_cosf_linear``: Linear interpolation
   - ``hpm_math_fast_sinf_quadratic`` / ``hpm_math_fast_cosf_quadratic``: Quadratic interpolation (most accurate)

2. **SinCos Functions** (optimized for simultaneous sin+cos calculation)

   - ``hpm_math_fast_sincosf_linear``: Compute sin and cos together with linear interpolation
   - ``hpm_math_fast_sincosf_quadratic``: Compute sin and cos together with quadratic interpolation
   - Much faster than calling sinf and cosf separately (shares angle normalization and index calculation)

3. **Tan Functions** (using optimized sincosf)

   - ``hpm_math_fast_tanf_linear``: Linear interpolation
   - ``hpm_math_fast_tanf_quadratic``: Quadratic interpolation
   - ``hpm_math_fast_tanf_hybrid``: Hybrid method (highest accuracy, ~2.5x improvement)

     - Uses polynomial approximation near singularities to avoid division error amplification
     - Uses sin/cos division elsewhere for best overall accuracy

4. **Atan/Atan2 Functions** (polynomial with range reduction)

   - ``hpm_math_fast_atanf``: Fast arctangent with range reduction
   - ``hpm_math_fast_atan2f``: Fast two-argument arctangent

Test Items
----------

The test includes the following items:

**Performance Test**

Measures execution time (in CPU ticks) for each function over 1000 iterations.

1. Sin Functions

   - ``sinf`` (stdlib): Standard library sine function
   - ``hpm_math_fast_sinf``: Fast sine without interpolation
   - ``hpm_math_fast_sinf_linear``: Fast sine with linear interpolation
   - ``hpm_math_fast_sinf_quadratic``: Fast sine with quadratic interpolation

2. Cos Functions

   - ``cosf`` (stdlib): Standard library cosine function
   - ``hpm_math_fast_cosf``: Fast cosine without interpolation
   - ``hpm_math_fast_cosf_linear``: Fast cosine with linear interpolation
   - ``hpm_math_fast_cosf_quadratic``: Fast cosine with quadratic interpolation

3. SinCos Functions

   - ``hpm_math_fast_sincosf_linear``: Simultaneous sin+cos with linear interpolation
   - ``hpm_math_fast_sincosf_quadratic``: Simultaneous sin+cos with quadratic interpolation

4. Tan Functions

   - ``tanf`` (stdlib): Standard library tangent function
   - ``hpm_math_fast_tanf_linear``: Fast tangent with linear interpolation
   - ``hpm_math_fast_tanf_quadratic``: Fast tangent with quadratic interpolation
   - ``hpm_math_fast_tanf_hybrid``: Fast tangent with hybrid method

5. Atan Functions

   - ``atanf`` (stdlib): Standard library arctangent function
   - ``hpm_math_fast_atanf``: Fast arctangent with range reduction

6. Atan2 Functions

   - ``atan2f`` (stdlib): Standard library two-argument arctangent
   - ``hpm_math_fast_atan2f``: Fast two-argument arctangent

**Accuracy Test**

Measures maximum and average error compared to standard library over millions of test points.

1. Sin Accuracy: Tests 6,530,344 points in [0, 2π] with 1e-6 rad step
2. Cos Accuracy: Tests 6,530,344 points in [0, 2π] with 1e-6 rad step
3. Tan Accuracy: Tests 6,110,914 points in [0, 2π] avoiding singularities (``|x - π/2| > 0.1``)
4. Atan Accuracy: Tests 199,874 points in [-10, 10] with 0.0001 step
5. Atan2 Accuracy: Tests 6,284 points using unit circle coordinates

Performance Results
-------------------

The following results were obtained on HPM6E00EVK at 600MHz (Table size: 4096).

.. note::

   Performance may vary depending on the chip model, compiler optimization level,
   cache configuration, and other system settings. These results are for reference only.

**Sin Functions (ticks per 1000 operations)**

+----------------------------------+----------+----------+
| Function                         | Total    | Per Op   |
+==================================+==========+==========+
| sinf (stdlib)                    | 104053   | 104      |
+----------------------------------+----------+----------+
| hpm_math_fast_sinf               | 14118    | 14       |
+----------------------------------+----------+----------+
| hpm_math_fast_sinf_linear        | 26442    | 26       |
+----------------------------------+----------+----------+
| hpm_math_fast_sinf_quadratic     | 39815    | 40       |
+----------------------------------+----------+----------+

**Cos Functions (ticks per 1000 operations)**

+----------------------------------+----------+----------+
| Function                         | Total    | Per Op   |
+==================================+==========+==========+
| cosf (stdlib)                    | 106108   | 106      |
+----------------------------------+----------+----------+
| hpm_math_fast_cosf               | 18059    | 18       |
+----------------------------------+----------+----------+
| hpm_math_fast_cosf_linear        | 29426    | 29       |
+----------------------------------+----------+----------+
| hpm_math_fast_cosf_quadratic     | 43418    | 43       |
+----------------------------------+----------+----------+

**SinCos Functions (ticks per 1000 operations)**

+----------------------------------+----------+----------+
| Function                         | Total    | Per Op   |
+==================================+==========+==========+
| hpm_math_fast_sincosf_linear     | 33480    | 33       |
+----------------------------------+----------+----------+
| hpm_math_fast_sincosf_quadratic  | 47801    | 48       |
+----------------------------------+----------+----------+

**Tan Functions (ticks per 1000 operations)**

+----------------------------------+----------+----------+
| Function                         | Total    | Per Op   |
+==================================+==========+==========+
| tanf (stdlib)                    | 144019   | 144      |
+----------------------------------+----------+----------+
| hpm_math_fast_tanf_linear        | 51416    | 51       |
+----------------------------------+----------+----------+
| hpm_math_fast_tanf_quadratic     | 65778    | 66       |
+----------------------------------+----------+----------+
| hpm_math_fast_tanf_hybrid        | 105441   | 105      |
+----------------------------------+----------+----------+

**Atan/Atan2 Functions (ticks per 1000 operations)**

+----------------------------------+----------+----------+
| Function                         | Total    | Per Op   |
+==================================+==========+==========+
| atanf (stdlib)                   | 178337   | 178      |
+----------------------------------+----------+----------+
| hpm_math_fast_atanf              | 58052    | 58       |
+----------------------------------+----------+----------+
| atan2f (stdlib)                  | 218747   | 218      |
+----------------------------------+----------+----------+
| hpm_math_fast_atan2f             | 57391    | 57       |
+----------------------------------+----------+----------+

Accuracy Results
----------------

**Sin Accuracy (6,530,344 test points)**

+-------------------------+-----------+-----------+
| Method                  | Max Error | Avg Error |
+=========================+===========+===========+
| No interpolation        | 1.53e-03  | 4.84e-04  |
+-------------------------+-----------+-----------+
| Linear interpolation    | 5.36e-07  | 1.46e-07  |
+-------------------------+-----------+-----------+
| Quadratic interpolation | 5.59e-07  | 6.41e-08  |
+-------------------------+-----------+-----------+

**Cos Accuracy (6,530,344 test points)**

+-------------------------+-----------+-----------+
| Method                  | Max Error | Avg Error |
+=========================+===========+===========+
| No interpolation        | 1.53e-03  | 4.87e-04  |
+-------------------------+-----------+-----------+
| Linear interpolation    | 6.56e-07  | 1.60e-07  |
+-------------------------+-----------+-----------+
| Quadratic interpolation | 5.36e-07  | 7.61e-08  |
+-------------------------+-----------+-----------+

**Tan Accuracy (6,110,914 test points, avoiding singularities)**

+-------------------------+-----------+-----------+
| Method                  | Max Error | Avg Error |
+=========================+===========+===========+
| Linear interpolation    | 3.91e-05  | 7.53e-07  |
+-------------------------+-----------+-----------+
| Quadratic interpolation | 3.91e-05  | 7.56e-07  |
+-------------------------+-----------+-----------+
| Hybrid method           | 1.53e-05  | 6.17e-07  |
+-------------------------+-----------+-----------+

**Atan Accuracy (199,874 test points, range [-10, 10])**

+-------------------------+-----------+-----------+
| Method                  | Max Error | Avg Error |
+=========================+===========+===========+
| Fast (range reduction)  | 5.01e-06  | 2.49e-07  |
+-------------------------+-----------+-----------+

**Atan2 Accuracy (6,284 test points)**

+-------------------------+-----------+-----------+
| Method                  | Max Error | Avg Error |
+=========================+===========+===========+
| Fast (range reduction)  | 5.01e-06  | 3.88e-07  |
+-------------------------+-----------+-----------+

Test Configuration
------------------

Error thresholds (configurable via macros):

- Sin/Cos no interpolation: 1e-2
- Sin/Cos linear interpolation: 1e-4
- Sin/Cos quadratic interpolation: 1e-5
- Tan linear: 1e-3
- Tan quadratic: 1e-4
- Tan hybrid: 2e-5
- Atan/Atan2: 1e-4

Usage
-----

The example automatically runs comprehensive tests and displays results on the debug console.
No user interaction is required.

License
-------

BSD-3-Clause
