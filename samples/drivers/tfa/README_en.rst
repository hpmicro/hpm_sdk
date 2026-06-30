.. _tfa:

TFA
======

Overview
--------

TFA is a hardware trigonometric function accelerator used to improve math operation performance.

Supported Functions
-------------------
- INV(x)
- SQRT(x)
- SIN(x)
- COS(x)
- POW(2,x)
- LOG2(x)
- ATAN(x)
- INVSQRT(x)
- ATAN2(y,x)
- CORDIC(x,y)

.. note::
   ATAN2 and CORDIC functions are only supported on some SoCs.
   On other SoCs, selecting these functions will print a "not supported" message.

Hardware Configuration
----------------------

No special hardware configuration required.

Toolchain Requirements
----------------------

This example requires a DSP-enabled toolchain:

- **IDE**: Segger Embedded Studio (or other IDEs with DSP toolchain support)
- **GCC Toolchain**: Andes RISC-V toolchain (with DSP extension) or ZCC toolchain (with DSP extension)

.. note::
   Standard GCC toolchains without DSP extensions cannot correctly compile and run this example.

Running the Program
-------------------

The program enables FPU by default for optimal performance, with an option to disable FPU.

Run Example (FPU enabled):

.. code-block:: console

   *********************************************************************************
   *                                                                               *
   *                         TFA Example Menu                                      *
   *                                                                               *
   * 0 - INV TEST                                                                  *
   * 1 - SQRT TEST                                                                 *
   * 2 - SIN TEST                                                                  *
   * 3 - COS TEST                                                                  *
   * 4 - POW2 TEST                                                                 *
   * 5 - LOG2 TEST                                                                 *
   * 6 - ATAN TEST                                                                 *
   * 7 - INVSQRT TEST                                                              *
   * 8 - ANANPU2 TEST                                                              *
   * 9 - CORDIC32 OPERATION                                                        *
   * A - MIXED OPERATION                                                           *
   *                                                                               *
   *********************************************************************************

Menu 0–9 test 10 TFA functions, comparing result and cycle time against the library. Results:

.. code-block:: console

   0
   tfa and math diff value:-0.000000, math calculation time:33 ticks, tfa calculation time:62 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:363 ticks, tfa calculation time:63 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:1755 ticks, tfa calculation time:63 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:1826 ticks, tfa calculation time:63 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:3031 ticks, tfa calculation time:63 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:2875 ticks, tfa calculation time:63 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:1959 ticks, tfa calculation time:180 ticks.
   7
   tfa and math diff value:-0.000000, math calculation time:552 ticks, tfa calculation time:63 ticks.
   8
   tfa and math diff value:0.000000, math calculation time:2108 ticks, tfa calculation time:150 ticks.
   9
   math angle value:6.981058, tfa angle value:6.980525, tfa and math angle diff value:-0.000533, math mode value:98.731964, tfa mode value:98.000000, tfa and math mode diff value:-0.731960, math calculation time:2901 ticks, tfa calculation time:93 ticks.

Menu option A performs a complex calculation, comparing its result and timing against the library function. Results:

.. code-block:: console

   A
   tfa and math diff value:-0.000000, math calculation time:9088 ticks, tfa calculation time:290 ticks.
