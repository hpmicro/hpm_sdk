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
   TFA supports up to 4 levels of nested computation. If the nesting exceeds 4 levels, the results of the outermost computation will be lost and will be recomputed.
   This project enables interrupt nesting protection by default.
   If User has confirmed that the current environment has already disabled interrupts, ensuring that no interrupt nesting will occur (which could otherwise cause ongoing computation to be interrupted and corrupted by data computation triggered in a new interrupt),
   the macro "CONFIG_TFA_NOT_USE_IRQ_NEST_PROTECTION" can be enabled in the CMakeLists file to disable the nesting protection for higher performance.


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
   tfa and math diff value:0.000000, math calculation time:24 ticks, tfa calculation time:57 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:25 ticks, tfa calculation time:58 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:307 ticks, tfa calculation time:58 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:309 ticks, tfa calculation time:58 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:358 ticks, tfa calculation time:58 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:291 ticks, tfa calculation time:58 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:535 ticks, tfa calculation time:58 ticks.
   7
   tfa and math diff value:0.000000, math calculation time:43 ticks, tfa calculation time:58 ticks.
   8
   tfa and math diff value:0.000000, math calculation time:513 ticks, tfa calculation time:121 ticks.
   9
   math angle value:6.981057, tfa angle value:6.980525, tfa and math angle diff value:-0.000533,math mode value:98.731964, tfa mode value:98.000000, tfa and math mode diff value:-0.731964,math calculation time:401 ticks, tfa calculation time:80 ticks.

Menu option A performs a complex calculation, comparing its result and timing against the library function. Results:

.. code-block:: console

   A
   tfa and math diff value:-0.000000, math calculation time:1269 ticks, tfa calculation time:288 ticks.

Note: The mathematical function powf used in Menu 4 POW2 TEST cannot calculate correct values under the zcc 4.1.5 toolchain. This issue has been fixed in zcc 4.1.8, and users can upgrade to version 4.1.8 for accurate computation.
