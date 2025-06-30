.. _pllctl_basic:

PLLCTL
============

Overview
--------

This example demonstrates the configuration and control of the Phase-Locked Loop (PLL) controller, showcasing how to manipulate system clock characteristics through various PLL parameters.

**Operating Modes**

The PLL supports two distinct operating modes:

- **Integer Mode**: Uses whole number divider ratios for simpler, fixed-frequency operation
- **Fractional Mode**: Enables fine-grained frequency control using fractional divider values

**Frequency Range**

- Operating Range: **380 MHz to 820 MHz** (380,000,000 Hz to 820,000,000 Hz)
- The example measures and displays the actual output frequency alongside the target frequency, providing insight into real-world frequency accuracy

Board Setting
-------------

- None

Running the example
-------------------

When the project runs correctly, the serial port terminal will output the following information:

.. code-block:: console

   PLLCTL example
   Now configure PLL at integer mode
   Update PLL0 to 380000000Hz @integer mode
   Actual f = 372000000Hz
   Update PLL0 to 400000000Hz @integer mode
   Actual f = 396000000Hz
   Update PLL0 to 420000000Hz @integer mode
   Actual f = 420000000Hz
   Update PLL0 to 440000000Hz @integer mode
   Actual f = 432000000Hz
   Update PLL0 to 460000000Hz @integer mode
   Actual f = 456000000Hz
   Update PLL0 to 480000000Hz @integer mode
   Actual f = 480000000Hz
   Update PLL0 to 500000000Hz @integer mode
   Actual f = 492000000Hz
   Update PLL0 to 520000000Hz @integer mode
   Actual f = 516000000Hz
   Update PLL0 to 540000000Hz @integer mode
   Actual f = 540000000Hz
   Update PLL0 to 560000000Hz @integer mode
   Actual f = 552000000Hz
   Update PLL0 to 580000000Hz @integer mode
   Actual f = 576000000Hz
   Update PLL0 to 600000000Hz @integer mode
   Actual f = 600000000Hz
   Update PLL0 to 620000000Hz @integer mode
   Actual f = 612000000Hz
   Update PLL0 to 640000000Hz @integer mode
   Actual f = 636000000Hz
   Update PLL0 to 660000000Hz @integer mode
   Actual f = 660000000Hz
   Update PLL0 to 680000000Hz @integer mode
   Actual f = 672000000Hz
   Update PLL0 to 700000000Hz @integer mode
   Actual f = 696000000Hz
   Update PLL0 to 720000000Hz @integer mode
   Actual f = 720000000Hz
   Update PLL0 to 740000000Hz @integer mode
   Actual f = 732000000Hz
   Update PLL0 to 760000000Hz @integer mode
   Actual f = 756000000Hz
   Update PLL0 to 780000000Hz @integer mode
   Actual f = 780000000Hz
   Update PLL0 to 800000000Hz @integer mode
   Actual f = 792000000Hz
   Update PLL0 to 820000000Hz @integer mode
   Actual f = 816000000Hz
   Now configure PLL at fraction mode
   Update PLL0 to 380000000Hz @fraction mode
   Actual f = 379999999Hz
   Update PLL0 to 400000000Hz @fraction mode
   Actual f = 399999999Hz
   Update PLL0 to 420000000Hz @fraction mode
   Actual f = 420000000Hz
   Update PLL0 to 440000000Hz @fraction mode
   Actual f = 439999999Hz
   Update PLL0 to 460000000Hz @fraction mode
   Actual f = 459999999Hz
   Update PLL0 to 480000000Hz @fraction mode
   Actual f = 480000000Hz
   Update PLL0 to 500000000Hz @fraction mode
   Actual f = 499999999Hz
   Update PLL0 to 520000000Hz @fraction mode
   Actual f = 519999999Hz
   Update PLL0 to 540000000Hz @fraction mode
   Actual f = 540000000Hz
   Update PLL0 to 560000000Hz @fraction mode
   Actual f = 559999999Hz
   Update PLL0 to 580000000Hz @fraction mode
   Actual f = 579999999Hz
   Update PLL0 to 600000000Hz @fraction mode
   Actual f = 600000000Hz
   Update PLL0 to 620000000Hz @fraction mode
   Actual f = 619999999Hz
   Update PLL0 to 640000000Hz @fraction mode
   Actual f = 639999999Hz
   Update PLL0 to 660000000Hz @fraction mode
   Actual f = 660000000Hz
   Update PLL0 to 680000000Hz @fraction mode
   Actual f = 679999999Hz
   Update PLL0 to 700000000Hz @fraction mode
   Actual f = 699999999Hz
   Update PLL0 to 720000000Hz @fraction mode
   Actual f = 720000000Hz
   Update PLL0 to 740000000Hz @fraction mode
   Actual f = 739999999Hz
   Update PLL0 to 760000000Hz @fraction mode
   Actual f = 759999999Hz
   Update PLL0 to 780000000Hz @fraction mode
   Actual f = 780000000Hz
   Update PLL0 to 800000000Hz @fraction mode
   Actual f = 799999999Hz
   Update PLL0 to 820000000Hz @fraction mode
   Actual f = 819999999Hz

   PLLCTL example finished

