.. _pllctlv2_basic:

PLLCTLV2 BASIC
================

Overview
--------

This example demonstrates the configuration and control of the Phase-Locked Loop (PLL) controller version 2, showcasing how to manipulate system clock characteristics through PLL parameters.

**Adjusting Frequency Range**

- Set Frequency from **400 MHz to 1 GHz**
- The example displays the actual output frequency alongside the target frequency.

Board Setting
-------------

- None

Running the example
-------------------

When the project runs correctly, the serial port terminal will output the following information:

.. code-block:: console

   PLLCTLV2 basic example
   Now configure PLL at integer mode
   Update PLL2 to 400000000Hz
   Actual PLL frequency is 400000000Hz
   Update PLL2 to 401000000Hz
   Actual PLL frequency is 401000000Hz
   Update PLL2 to 402000000Hz
   Actual PLL frequency is 402000000Hz
   Update PLL2 to 403000000Hz
   Actual PLL frequency is 403000000Hz
   Update PLL2 to 404000000Hz
   Actual PLL frequency is 404000000Hz
   Update PLL2 to 405000000Hz
   Actual PLL frequency is 405000000Hz
   Update PLL2 to 406000000Hz
   Actual PLL frequency is 406000000Hz
   Update PLL2 to 407000000Hz
   ...
   Update PLL2 to 998000000Hz
   Actual PLL frequency is 998000000Hz
   Update PLL2 to 999000000Hz
   Actual PLL frequency is 999000000Hz
   Update PLL2 to 1000000000Hz
   Actual PLL frequency is 1000000000Hz

   PLLCTLV2 basic example finished
